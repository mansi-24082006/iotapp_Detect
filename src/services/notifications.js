import { Alert, Platform } from 'react-native';
import messaging from '@react-native-firebase/messaging';
import { db } from './firebase';

const TOKEN_PATH = '/deviceTokens';

export const initMessaging = async () => {
  const authStatus = await messaging().requestPermission();
  const enabled =
    authStatus === messaging.AuthorizationStatus.AUTHORIZED ||
    authStatus === messaging.AuthorizationStatus.PROVISIONAL;

  if (!enabled) {
    return;
  }

  const token = await messaging().getToken();
  if (token) {
    const tokenKey = token.replace(/\./g, ',');
    await db.ref(`${TOKEN_PATH}/${tokenKey}`).set({
      token,
      platform: Platform.OS,
      updatedAt: Date.now(),
      active: true,
    });
  }

  messaging().onTokenRefresh(async nextToken => {
    const nextTokenKey = nextToken.replace(/\./g, ',');
    await db.ref(`${TOKEN_PATH}/${nextTokenKey}`).set({
      token: nextToken,
      platform: Platform.OS,
      updatedAt: Date.now(),
      active: true,
    });
  });

  messaging().onMessage(async remoteMessage => {
    const title = remoteMessage?.notification?.title || 'Gas Alert';
    const body = remoteMessage?.notification?.body || 'New IoT alert received.';
    Alert.alert(title, body);
  });
};

export const handleBackgroundMessage = async remoteMessage => {
  const alertType = remoteMessage?.data?.type;
  if (alertType) {
    // Keep handler to ensure data-only pushes are processed in background.
    await Promise.resolve(alertType);
  }
};
