import React, { useEffect } from 'react';
import { SafeAreaProvider } from 'react-native-safe-area-context';
import { AppNavigator } from './src/navigation/AppNavigator';
import { useIoTStore } from './src/store/useIoTStore';

export default function App() {
  const initListeners = useIoTStore((state) => state.initListeners);

  useEffect(() => {
    // Initialize Firebase Realtime Listeners natively
    initListeners();
  }, []);

  return (
    <SafeAreaProvider>
      <AppNavigator />
    </SafeAreaProvider>
  );
}
