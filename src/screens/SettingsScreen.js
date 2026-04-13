import React, { useEffect, useState } from 'react';
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
import LinearGradient from 'react-native-linear-gradient';

export const SettingsScreen = () => {
  const { settings, updateSettings } = useIoTStore();
  const [localThreshold, setLocalThreshold] = useState(
    settings.threshold.toString(),
  );

  useEffect(() => {
    setLocalThreshold(settings.threshold.toString());
  }, [settings.threshold]);

  const handleSave = () => {
    updateSettings({
      threshold: parseInt(localThreshold, 10) || 1350,
    });
  };

  return (
    <LinearGradient
      colors={[colors.backgroundAlt, colors.background]}
      style={styles.container}
    >
      <Text style={styles.header}>Settings</Text>
      <Text style={styles.subtitle}>
        Tune the alert threshold and optional notification behavior for your gas
        system.
      </Text>

      <View style={styles.card}>
        <Text style={styles.label}>Gas Alert Threshold</Text>
        <Text style={styles.helper}>
          Lower values trigger the alert sooner. Current value:{' '}
          {settings.threshold}
        </Text>
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
          <View style={styles.textWrap}>
            <Text style={styles.label}>Enable Notifications</Text>
            <Text style={styles.helper}>
              Allows Firebase alerts and in-app warning prompts.
            </Text>
          </View>
          <Switch
            value={settings.notificationsEnabled}
            onValueChange={val => updateSettings({ notificationsEnabled: val })}
            trackColor={{ false: colors.surfaceLight, true: colors.primary }}
          />
        </View>
        <View style={[styles.row, { marginTop: 20 }]}>
          <View style={styles.textWrap}>
            <Text style={styles.label}>Emergency Auto-Call</Text>
            <Text style={styles.helper}>
              Call the configured number when the alert state is active.
            </Text>
          </View>
          <Switch
            value={settings.emergencyCallEnabled}
            onValueChange={val => updateSettings({ emergencyCallEnabled: val })}
            trackColor={{ false: colors.surfaceLight, true: colors.danger }}
          />
        </View>
      </View>
    </LinearGradient>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    padding: 20,
    paddingTop: 56,
  },
  header: {
    fontSize: 30,
    fontWeight: '900',
    color: colors.textPrimary,
  },
  subtitle: {
    color: colors.textSecondary,
    fontSize: 14,
    lineHeight: 20,
    marginTop: 8,
    marginBottom: 22,
  },
  card: {
    backgroundColor: colors.surface,
    padding: 18,
    borderRadius: 24,
    borderWidth: 1,
    borderColor: colors.border,
    marginBottom: 16,
  },
  label: {
    color: colors.textPrimary,
    fontSize: 16,
    fontWeight: '800',
  },
  helper: {
    color: colors.textSecondary,
    fontSize: 13,
    lineHeight: 18,
    marginTop: 6,
    marginBottom: 12,
  },
  input: {
    backgroundColor: colors.surfaceLight,
    color: colors.textPrimary,
    paddingVertical: 14,
    paddingHorizontal: 16,
    borderRadius: 14,
    fontSize: 16,
    marginBottom: 15,
    borderWidth: 1,
    borderColor: colors.border,
  },
  button: {
    backgroundColor: colors.primary,
    paddingVertical: 14,
    borderRadius: 14,
    alignItems: 'center',
  },
  buttonText: {
    color: colors.background,
    fontWeight: '900',
    fontSize: 16,
  },
  row: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    gap: 16,
  },
  textWrap: {
    flex: 1,
  },
  modeToggleWrap: {
    alignItems: 'center',
    justifyContent: 'center',
  },
  modeLabel: {
    color: colors.textMuted,
    fontSize: 10,
    fontWeight: '900',
    marginBottom: 4,
  },
});
