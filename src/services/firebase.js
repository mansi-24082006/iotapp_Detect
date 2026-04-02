import { initializeApp, getApp, getApps } from '@react-native-firebase/app';
import database from '@react-native-firebase/database';

// In bare React Native, Firebase automatically initializes from your 
// google-services.json file inside android/app/!
// You DO NOT need to hardcode API keys here anymore.
export const db = database();
