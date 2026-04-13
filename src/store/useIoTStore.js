import { create } from 'zustand';
import { db } from '../services/firebase';
import { initMessaging } from '../services/notifications';

const DEFAULT_DEVICE = {
  gasLevel: 0,
  threshold: 1600,
  gasDetected: false,
  sensorWarmedUp: false,
  eventLatched: false,
  status: 'SAFE',
  state: 'NORMAL',
  fan: false,
  buzzer: false,
  servo: false,
  rawStatus: 'NORMAL',
  lastUpdated: 0,
};

const KNOWN_STATES = new Set([
  'NORMAL',
  'WARMING_UP',
  'GAS_DETECTED',
  'WAITING_MANUAL_RESET',
]);

const deriveDeviceState = (device, threshold) => {
  const gasLevel = Number(device?.gasValue ?? device?.gasLevel ?? 0);
  const cloudThreshold = Number(device?.threshold ?? threshold ?? 1800);
  // Support both /sensor/state structure and /device folder structure
  const isLatched = Boolean(device?.latched ?? (device?.status === 'WARNING'));
  const isDanger = Boolean(device?.danger ?? (device?.status === 'DANGER'));
  const rawStatus = typeof device?.status === 'string' ? device.status : 'NORMAL';

  // State mapping for UI
  const state = isDanger ? 'GAS_DETECTED' : (isLatched ? 'WARNING' : 'NORMAL');
  const statusLabel = rawStatus === 'DANGER' ? 'DANGER' : (rawStatus === 'WARNING' ? 'RESET REQUIRED' : 'SAFE');

  const fan = Boolean(device?.fanOn ?? device?.fan ?? false);
  const servo = Boolean(device?.servoClosed ?? device?.servo ?? false);
  const buzzer = Boolean(device?.buzzerOn ?? device?.buzzer ?? false);

  return {
    ...DEFAULT_DEVICE,
    ...device,
    gasLevel,
    threshold: cloudThreshold,
    gasDetected: isDanger,
    eventLatched: isLatched,
    status: statusLabel,
    state,
    buzzer,
    fan,
    servo,
  };
};

export const useIoTStore = create((set, get) => ({
  device: DEFAULT_DEVICE,
  settings: {
    threshold: 1800,
    notificationsEnabled: true,
  },
  isConnected: false,

  initListeners: () => {
    // Connection State
    const connectedRef = db.ref('.info/connected');
    connectedRef.on('value', snap => {
      set({ isConnected: snap.val() === true });
    });

    // Main Status Listener (/sensor/state or /device)
    const deviceRef = db.ref('/device');
    deviceRef.on('value', snapshot => {
      const data = snapshot.val();
      if (data) {
        set(state => ({
          device: deriveDeviceState(
            { ...state.device, ...data },
            state.settings.threshold,
          ),
        }));
      }
    });

    // Dedicated Live Gas Listener
    const gasRef = db.ref('/sensor/gas');
    gasRef.on('value', snapshot => {
      const gasValue = snapshot.val();
      if (gasValue !== null) {
        set(state => ({
          device: { ...state.device, gasLevel: Number(gasValue) }
        }));
      }
    });

    // Settings Listener
    const settingsRef = db.ref('/settings');
    settingsRef.on('value', snapshot => {
      const data = snapshot.val();
      if (data) {
        set(state => {
          const nextSettings = { ...state.settings, ...data };
          return {
            settings: nextSettings,
            device: deriveDeviceState(state.device, nextSettings.threshold),
          };
        });
      }
    });
  },

  initNotifications: async () => {
    try {
      await initMessaging();
    } catch (e) {
      console.error('Failed to initialize notifications', e);
    }
  },

  updateDeviceToggle: async (key, value) => {
    try {
      const { device } = get();
      // Allow manual toggle only if gas is safe but we are in the latched state
      if (!device.gasDetected && device.eventLatched) {
        if (key === 'fan') await db.ref('/controls/fan').set(value);
        if (key === 'servo') await db.ref('/controls/servo').set(value);
      }
    } catch (e) {
      console.error('Failed to update toggle', e);
    }
  },

  triggerReset: async type => {
    try {
      const { device } = get();
      // Only allow reset if gas is cleared
      if (device.gasDetected) {
        return;
      }

      if (type === 'all' || type === 'reset') {
        await db.ref('/controls/reset').set(true);
      }
    } catch (e) {
      console.error('Failed to trigger reset', e);
    }
  },

  updateSettings: async newSettings => {
    try {
      set(state => ({
        settings: { ...state.settings, ...newSettings }
      }));
      await db.ref('/settings').update(newSettings);
    } catch (e) {
      console.error('Failed to update settings', e);
    }
  },
}));
