import React, { useEffect, useRef } from 'react';
import { Alert } from 'react-native';
import { SafeAreaProvider } from 'react-native-safe-area-context';
import { AppNavigator } from './src/navigation/AppNavigator';
import { useIoTStore } from './src/store/useIoTStore';

export default function App() {
  const initListeners = useIoTStore((state) => state.initListeners);
  const initNotifications = useIoTStore((state) => state.initNotifications);
  const deviceState = useIoTStore((state) => state.device.state);
  const notificationsEnabled = useIoTStore(
    (state) => state.settings.notificationsEnabled,
  );
  const previousStateRef = useRef(deviceState);

  useEffect(() => {
    // Initialize Firebase Realtime Listeners natively
    initListeners();
    initNotifications();
  }, []);

  useEffect(() => {
    const previousState = previousStateRef.current;
    if (
      notificationsEnabled &&
      previousState !== 'GAS_DETECTED' &&
      deviceState === 'GAS_DETECTED'
    ) {
      Alert.alert(
        'Gas Alert',
        'Gas detected. Buzzer ON, servo 90 degrees, and fan ON.',
      );
    }
    previousStateRef.current = deviceState;
  }, [deviceState, notificationsEnabled]);

  return (
    <SafeAreaProvider>
      <AppNavigator />
    </SafeAreaProvider>
  );
}
