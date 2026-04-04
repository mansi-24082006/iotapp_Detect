import React, { useState } from 'react';
import {
  View,
  Text,
  StyleSheet,
  TextInput,
  Switch,
  Pressable,
} from 'react-native';
import { useIoTStore } from '../store/useIoTStore';
import { colors } from '../theme/colors';

export const SettingsScreen = () => {
  const { settings, updateSettings } = useIoTStore();
  const [localThreshold, setLocalThreshold] = useState(
    settings.threshold.toString(),
  );

  const handleSave = () => {
    updateSettings({
      threshold: parseInt(localThreshold, 10) || 1350,
    });
  };

  return (
    <View style={styles.container}>
      <Text style={styles.header}>Settings</Text>

      <View style={styles.card}>
        <Text style={styles.label}>Gas Alert Threshold</Text>
        <TextInput
          style={styles.input}
          value={localThreshold}
          onChangeText={setLocalThreshold}
          keyboardType="numeric"
          placeholderTextColor={colors.textSecondary}
        />
        <Pressable style={styles.button} onPress={handleSave}>
          <Text style={styles.buttonText}>Save Threshold</Text>
        </Pressable>
      </View>

      <View style={styles.card}>
        <View style={styles.row}>
          <Text style={styles.label}>Enable Notifications</Text>
          <Switch
            value={settings.notificationsEnabled}
            onValueChange={val => updateSettings({ notificationsEnabled: val })}
            trackColor={{ false: colors.surfaceLight, true: colors.primary }}
          />
        </View>
        <View style={[styles.row, { marginTop: 20 }]}>
          <Text style={styles.label}>Emergency Auto-Call</Text>
          <Switch
            value={settings.emergencyCallEnabled}
            onValueChange={val => updateSettings({ emergencyCallEnabled: val })}
            trackColor={{ false: colors.surfaceLight, true: colors.danger }}
          />
        </View>
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: colors.background,
    padding: 24,
    paddingTop: 60,
  },
  header: {
    fontSize: 28,
    fontWeight: 'bold',
    color: colors.textPrimary,
    marginBottom: 30,
  },
  card: {
    backgroundColor: colors.surface,
    padding: 20,
    borderRadius: 20,
    borderWidth: 1,
    borderColor: colors.border,
    marginBottom: 20,
  },
  label: {
    color: colors.textPrimary,
    fontSize: 16,
    marginBottom: 10,
  },
  input: {
    backgroundColor: colors.surfaceLight,
    color: colors.textPrimary,
    padding: 15,
    borderRadius: 10,
    fontSize: 16,
    marginBottom: 15,
  },
  button: {
    backgroundColor: colors.primary,
    padding: 15,
    borderRadius: 10,
    alignItems: 'center',
  },
  buttonText: {
    color: colors.background,
    fontWeight: 'bold',
    fontSize: 16,
  },
  row: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
  },
});
