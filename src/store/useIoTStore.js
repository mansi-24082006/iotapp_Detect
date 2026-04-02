import { create } from 'zustand';
import { db } from '../services/firebase';

export const useIoTStore = create((set, get) => ({
  device: {
    gasLevel: 0,
    status: 'SAFE',
    fan: false,
    buzzer: false,
    servo: false,
    lastUpdated: 0,
  },
  settings: {
    threshold: 1350,
    notificationsEnabled: true,
    emergencyCallEnabled: false,
    emergencyNumber: '',
  },
  isConnected: false,

  initListeners: () => {
    // Listen for connection state using Native Firebase string refs
    const connectedRef = db.ref('.info/connected');
    connectedRef.on('value', (snap) => {
      set({ isConnected: snap.val() === true });
    });

    // Listen to /device
    const deviceRef = db.ref('/device');
    deviceRef.on('value', (snapshot) => {
      const data = snapshot.val();
      if (data) {
        set((state) => ({ device: { ...state.device, ...data } }));
      }
    });

    // Listen to /settings
    const settingsRef = db.ref('/settings');
    settingsRef.on('value', (snapshot) => {
      const data = snapshot.val();
      if (data) {
        set((state) => ({ settings: { ...state.settings, ...data } }));
      }
    });
  },

  updateDeviceToggle: async (key, value) => {
    try {
      await db.ref(`/device/${key}`).set(value);
    } catch (e) {
      console.error("Failed to update toggle", e);
    }
  },

  updateSettings: async (newSettings) => {
    try {
      await db.ref('/settings').update(newSettings);
    } catch (e) {
      console.error("Failed to update settings", e);
    }
  }
}));
