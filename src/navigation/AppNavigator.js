import React, { useEffect, useRef } from 'react';
import { NavigationContainer } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import { DashboardScreen } from '../screens/DashboardScreen';
import { AlertScreen } from '../screens/AlertScreen';
import { SettingsScreen } from '../screens/SettingsScreen';
import { useIoTStore } from '../store/useIoTStore';
import { colors } from '../theme/colors';

const Stack = createNativeStackNavigator();

export const AppNavigator = () => {
  const { device } = useIoTStore();
  const navigationRef = useRef(null);

  // Core requirement: Auto redirection on danger
  useEffect(() => {
    if (!navigationRef.current) return;
    
    if (device.status === 'DANGER') {
      // Force navigation to alert screen
      navigationRef.current.navigate('Alert');
    } else if (device.status === 'SAFE') {
      // Return to Dashboard if safe
      const currentRoute = navigationRef.current.getCurrentRoute();
      if (currentRoute && currentRoute.name === 'Alert') {
        navigationRef.current.navigate('Dashboard');
      }
    }
  }, [device.status]);

  return (
    <NavigationContainer ref={navigationRef}>
      <Stack.Navigator
        screenOptions={{
          headerShown: false,
          contentStyle: { backgroundColor: colors.background }
        }}
      >
        <Stack.Screen name="Dashboard" component={DashboardScreen} />
        <Stack.Screen 
          name="Alert" 
          component={AlertScreen} 
          options={{ gestureEnabled: false }} // Cannot swipe back from danger
        />
        <Stack.Screen name="Settings" component={SettingsScreen} />
      </Stack.Navigator>
    </NavigationContainer>
  );
};
