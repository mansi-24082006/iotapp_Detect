import React from 'react';
import { View, Text, StyleSheet, ScrollView, Pressable } from 'react-native';
import { useNavigation } from '@react-navigation/native';
import {
  Fan,
  RotateCcw,
  ShieldCheck,
  Settings as SettingsIcon,
  Radio,
  ShieldAlert,
  Thermometer,
} from 'lucide-react-native';
import { useIoTStore } from '../store/useIoTStore';
import { colors } from '../theme/colors';
import { AnimatedGauge } from '../components/AnimatedGauge';
import LinearGradient from 'react-native-linear-gradient';

export const DashboardScreen = () => {
  const { device, triggerReset, isConnected, updateDeviceToggle } = useIoTStore();
  const navigation = useNavigation();
  const needsReset = isConnected && !device.gasDetected && device.eventLatched;
  const isPostLeak = device.state === 'WARNING';
  
  const modeLabel =
    device.state === 'GAS_DETECTED'
      ? 'GAS DETECTED'
      : device.state === 'WARNING'
      ? 'POST-LEAK LATCHED'
      : 'MONITORING';

  const modeDescription =
    device.state === 'GAS_DETECTED'
      ? 'Automatic protection is active. Buzzer and Fan ON.'
      : device.state === 'WARNING'
      ? 'Gas level is safe now, but system requires a manual reset to reopen valve.'
      : 'All systems normal. Gas levels are being monitored.';

  return (
    <LinearGradient
      colors={[colors.backgroundAlt, colors.background]}
      style={styles.container}
    >
      <ScrollView contentContainerStyle={styles.scroll}>
        <View style={styles.header}>
          <View style={styles.headerCopy}>
            <Text style={styles.title}>Smart Gas</Text>
            <Text style={styles.subtitle}>{modeDescription}</Text>
          </View>
          <Pressable
            style={styles.settingsButton}
            onPress={() => navigation.navigate('Settings')}
          >
            <SettingsIcon color={colors.textPrimary} size={22} />
          </Pressable>
        </View>

        <View style={styles.banner}>
          <View>
            <Text style={styles.bannerLabel}>SYSTEM STATE</Text>
            <Text style={styles.bannerValue}>{modeLabel}</Text>
          </View>
          <View style={styles.connectionBadgeWrap}>
            <View
              style={[
                styles.connectionBadge,
                {
                  backgroundColor: isConnected ? colors.success : colors.danger,
                },
              ]}
            >
              <Text style={styles.connectionText}>
                {isConnected ? 'LIVE' : 'OFFLINE'}
              </Text>
            </View>
          </View>
        </View>

        <View style={styles.card3d}>
          <AnimatedGauge value={device.gasLevel} status={device.status} />
          <View style={styles.indicatorContainer}>
            <View style={styles.indicatorRow}>
              <Radio color={colors.primary} size={18} />
              <Text style={styles.indicatorLabel}>Status: {device.status}</Text>
            </View>
          </View>
        </View>

        <View style={styles.statsGrid}>
          <View style={styles.statCard}>
            <Thermometer color={colors.primary} size={18} />
            <Text style={styles.statLabel}>Current Level</Text>
            <Text style={styles.statValue}>{device.gasLevel}</Text>
          </View>
          <Pressable 
            style={[styles.statCard, isPostLeak && styles.interactiveCard]}
            onPress={() => isPostLeak && updateDeviceToggle('fan', !device.fan)}
          >
            <Fan
              color={device.fan ? colors.primary : colors.textMuted}
              size={18}
            />
            <Text style={styles.statLabel}>Fan Relay {isPostLeak ? '(Tap)' : ''}</Text>
            <Text style={styles.statValue}>{device.fan ? 'ACTIVE' : 'OFF'}</Text>
          </Pressable>
          <Pressable 
            style={[styles.statCard, isPostLeak && styles.interactiveCard]}
            onPress={() => isPostLeak && updateDeviceToggle('servo', !device.servo)}
          >
            <RotateCcw
              color={device.servo ? colors.primary : colors.textMuted}
              size={18}
            />
            <Text style={styles.statLabel}>Gas Valve {isPostLeak ? '(Tap)' : ''}</Text>
            <Text style={styles.statValue}>{device.servo ? 'CLOSED' : 'OPEN'}</Text>
          </Pressable>
          <View style={styles.statCard}>
            <ShieldAlert color={device.buzzer ? colors.danger : colors.textMuted} size={18} />
            <Text style={styles.statLabel}>Buzzer</Text>
            <Text style={styles.statValue}>{device.buzzer ? 'ON' : 'OFF'}</Text>
          </View>
        </View>

        {needsReset && (
          <View style={styles.controlsCard}>
            <View style={styles.sectionHeader}>
              <Text style={styles.sectionTitle}>Emergency Reset Required</Text>
              <Text style={styles.sectionHint}>
                Gas level is now safe. Press reset to reopen the valve and stop the fan.
              </Text>
            </View>
            <Pressable
              style={[styles.resetButton, styles.resetAll]}
              onPress={() => triggerReset('reset')}
            >
              <ShieldCheck color={colors.textPrimary} size={18} />
              <Text style={styles.resetButtonText}>Perform System Reset</Text>
            </Pressable>
          </View>
        )}

        <View style={styles.noteCard}>
          <Text style={styles.noteTitle}>Threshold Info</Text>
          <Text style={styles.noteText}>
            Alert triggers at {device.threshold}+. System will lock output states until manual reset for safety.
          </Text>
        </View>
      </ScrollView>
    </LinearGradient>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  scroll: {
    padding: 20,
    paddingTop: 56,
    paddingBottom: 28,
  },
  header: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: 20,
  },
  headerCopy: {
    flex: 1,
    paddingRight: 12,
  },
  title: {
    color: colors.textPrimary,
    fontSize: 30,
    fontWeight: '900',
    letterSpacing: 0.3,
  },
  subtitle: {
    color: colors.textSecondary,
    fontSize: 14,
    marginTop: 8,
    lineHeight: 20,
  },
  settingsButton: {
    width: 44,
    height: 44,
    borderRadius: 22,
    alignItems: 'center',
    justifyContent: 'center',
    backgroundColor: colors.surfaceLight,
    borderWidth: 1,
    borderColor: colors.border,
  },
  banner: {
    backgroundColor: colors.surfaceElevated,
    borderRadius: 24,
    padding: 18,
    marginBottom: 18,
    borderWidth: 1,
    borderColor: colors.border,
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
  },
  bannerLabel: {
    color: colors.textMuted,
    fontSize: 11,
    letterSpacing: 1.4,
    marginBottom: 6,
  },
  bannerValue: {
    color: colors.textPrimary,
    fontSize: 22,
    fontWeight: '900',
    letterSpacing: 0.4,
  },
  connectionBadgeWrap: {
    marginLeft: 12,
  },
  connectionBadge: {
    paddingHorizontal: 12,
    paddingVertical: 8,
    borderRadius: 999,
  },
  connectionText: {
    color: colors.background,
    fontWeight: '900',
    fontSize: 11,
    letterSpacing: 0.8,
  },
  card3d: {
    backgroundColor: colors.surfaceLight,
    borderRadius: 30,
    padding: 20,
    marginBottom: 18,
    shadowColor: colors.primaryGlow,
    shadowOffset: { width: 0, height: 20 },
    shadowOpacity: 0.3,
    shadowRadius: 30,
    elevation: 20,
    borderWidth: 1,
    borderColor: colors.border,
  },
  indicatorContainer: {
    flexDirection: 'row',
    justifyContent: 'center',
    alignItems: 'center',
    marginTop: 10,
    paddingTop: 18,
    borderTopWidth: 1,
    borderTopColor: colors.border,
  },
  indicatorRow: {
    flexDirection: 'row',
    alignItems: 'center',
    marginRight: 10,
  },
  indicatorLabel: {
    color: colors.textSecondary,
    fontSize: 16,
    marginLeft: 8,
  },
  indicatorValue: {
    fontSize: 18,
    fontWeight: '900',
    letterSpacing: 1,
  },
  statsGrid: {
    flexDirection: 'row',
    flexWrap: 'wrap',
    justifyContent: 'space-between',
    gap: 12,
    marginBottom: 18,
  },
  statCard: {
    width: '48%',
    backgroundColor: colors.surface,
    borderRadius: 22,
    padding: 16,
    borderWidth: 1,
    borderColor: colors.border,
    minHeight: 110,
  },
  interactiveCard: {
    borderColor: colors.primary,
    backgroundColor: colors.surfaceLight,
    borderWidth: 1.5,
  },
  statLabel: {
    color: colors.textSecondary,
    fontSize: 13,
    marginTop: 18,
  },
  statValue: {
    color: colors.textPrimary,
    fontSize: 22,
    fontWeight: '900',
    marginTop: 8,
  },
  controlsCard: {
    backgroundColor: colors.surface,
    borderRadius: 30,
    padding: 18,
    borderWidth: 1,
    borderColor: colors.border,
    marginBottom: 20,
  },
  sectionHeader: {
    marginBottom: 12,
  },
  sectionTitle: {
    color: colors.textPrimary,
    fontSize: 18,
    fontWeight: '800',
  },
  sectionHint: {
    color: colors.textMuted,
    fontSize: 13,
    lineHeight: 18,
    marginTop: 6,
  },
  noteCard: {
    marginTop: 8,
    borderRadius: 18,
    backgroundColor: colors.surfaceLight,
    padding: 14,
    borderWidth: 1,
    borderColor: colors.border,
  },
  noteTitle: {
    color: colors.textPrimary,
    fontWeight: '800',
    fontSize: 13,
    marginBottom: 6,
  },
  noteText: {
    color: colors.textSecondary,
    lineHeight: 20,
  },
  resetButton: {
    marginBottom: 10,
    borderRadius: 14,
    backgroundColor: colors.primary,
    paddingVertical: 12,
    paddingHorizontal: 14,
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'center',
    gap: 8,
  },
  resetButtonDisabled: {
    opacity: 0.45,
  },
  resetAll: {
    backgroundColor: colors.warning,
  },
  resetButtonText: {
    color: colors.textPrimary,
    fontWeight: '900',
    fontSize: 14,
  },
});
