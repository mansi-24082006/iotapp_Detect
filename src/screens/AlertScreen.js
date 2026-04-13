import React from 'react';
import { View, Text, StyleSheet, Pressable, ScrollView } from 'react-native';
import { useNavigation } from '@react-navigation/native';
import { useIoTStore } from '../store/useIoTStore';
import { colors } from '../theme/colors';
import {
  AlertOctagon,
  PhoneCall,
  Settings as SettingsIcon,
  Fan,
  RotateCcw,
  ShieldCheck,
} from 'lucide-react-native';
import LinearGradient from 'react-native-linear-gradient';

export const AlertScreen = () => {
  const { device, settings, triggerReset, isConnected } = useIoTStore();
  const navigation = useNavigation();
  const gasLevel = device.gasLevel;
  const canReset = isConnected && !device.gasDetected && device.eventLatched;
  const buzzerLabel = device.buzzer ? 'ON' : 'OFF';
  const modeLabel =
    device.state === 'WARNING'
      ? 'POST-ALERT'
      : device.state === 'WARMING_UP'
      ? 'WARMING UP'
      : 'GAS DETECTED';

  return (
    <LinearGradient
      colors={[colors.dangerBackground, colors.background]}
      style={styles.container}
    >
      <ScrollView contentContainerStyle={styles.content}>
        <View style={styles.headerRow}>
          <View style={styles.headerTextBlock}>
            <Text style={styles.headerLabel}>LIVE ALERT</Text>
            <Text style={styles.title}>Gas detected</Text>
            <Text style={styles.subtitle}>
              Automatic safety is active and this screen tracks the live
              response state.
            </Text>
          </View>
          <Pressable
            style={styles.settingsButton}
            onPress={() => navigation.navigate('Settings')}
          >
            <SettingsIcon color={colors.textPrimary} size={22} />
          </Pressable>
        </View>

        <View style={styles.iconWrap}>
          <AlertOctagon size={112} color={colors.danger} />
        </View>

        <View style={styles.banner}>
          <Text style={styles.bannerLabel}>SYSTEM MODE</Text>
          <Text style={styles.bannerValue}>{modeLabel}</Text>
          <Text style={styles.bannerText}>
            Gas detected keeps buzzer ON, fan ON, and servo at 90°. After gas
            drops, buzzer stays OFF and you can reset servo/fan from mobile.
          </Text>
        </View>

        <View style={styles.card}>
          <Text style={styles.cardLabel}>GAS LEVEL</Text>
          <Text style={styles.gasLevel}>{gasLevel} PPM</Text>
          <Text style={styles.info}>
            Keep the area ventilated. Use the controls below to adjust fan and
            servo if needed.
          </Text>
        </View>

        <View style={styles.statsRow}>
          <View style={styles.statBox}>
            <Text style={styles.statLabel}>Fan</Text>
            <Text style={styles.statValue}>{device.fan ? 'ON' : 'OFF'}</Text>
          </View>
          <View style={styles.statBox}>
            <Text style={styles.statLabel}>Servo</Text>
            <Text style={styles.statValue}>{device.servo ? '90°' : '0°'}</Text>
          </View>
          <View style={styles.statBox}>
            <Text style={styles.statLabel}>Buzzer</Text>
            <Text style={styles.statValue}>{buzzerLabel}</Text>
          </View>
        </View>

        <View style={styles.controlCard}>
          <Text style={styles.sectionTitle}>Manual Reset</Text>
          <Text style={styles.sectionText}>
            Reset commands are enabled only after gas drops below the threshold.
          </Text>
          <Pressable
            style={[
              styles.resetButton,
              !canReset && styles.resetButtonDisabled,
            ]}
            onPress={() => triggerReset('fan')}
            disabled={!canReset}
          >
            <Fan color={colors.textPrimary} size={18} />
            <Text style={styles.resetButtonText}>Reset Fan</Text>
          </Pressable>
          <Pressable
            style={[
              styles.resetButton,
              !canReset && styles.resetButtonDisabled,
            ]}
            onPress={() => triggerReset('servo')}
            disabled={!canReset}
          >
            <RotateCcw color={colors.textPrimary} size={18} />
            <Text style={styles.resetButtonText}>Reset Servo</Text>
          </Pressable>
          <Pressable
            style={[
              styles.resetButton,
              styles.resetAll,
              !canReset && styles.resetButtonDisabled,
            ]}
            onPress={() => triggerReset('all')}
            disabled={!canReset}
          >
            <ShieldCheck color={colors.textPrimary} size={18} />
            <Text style={styles.resetButtonText}>Reset All</Text>
          </Pressable>
        </View>

        {settings.emergencyCallEnabled && (
          <Pressable style={styles.callButton}>
            <PhoneCall color={colors.textPrimary} size={24} />
            <Text style={styles.callButtonText}>
              Call {settings.emergencyNumber}
            </Text>
          </Pressable>
        )}
      </ScrollView>
    </LinearGradient>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  content: {
    padding: 20,
    paddingTop: 56,
    paddingBottom: 30,
  },
  headerRow: {
    flexDirection: 'row',
    alignItems: 'flex-start',
    justifyContent: 'space-between',
    marginBottom: 16,
  },
  headerTextBlock: {
    flex: 1,
    paddingRight: 12,
  },
  headerLabel: {
    color: colors.warning,
    fontSize: 11,
    fontWeight: '900',
    letterSpacing: 1.6,
    marginBottom: 8,
  },
  title: {
    color: colors.danger,
    fontSize: 34,
    fontWeight: '900',
    letterSpacing: 0.5,
  },
  subtitle: {
    color: colors.textSecondary,
    fontSize: 14,
    lineHeight: 20,
    marginTop: 10,
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
  iconWrap: {
    alignItems: 'center',
    justifyContent: 'center',
    marginVertical: 14,
  },
  banner: {
    backgroundColor: 'rgba(255,94,120,0.12)',
    borderWidth: 1,
    borderColor: colors.danger,
    borderRadius: 24,
    padding: 18,
    marginBottom: 16,
  },
  bannerLabel: {
    color: colors.danger,
    fontSize: 11,
    letterSpacing: 1.4,
    fontWeight: '900',
    marginBottom: 8,
  },
  bannerValue: {
    color: colors.textPrimary,
    fontSize: 22,
    fontWeight: '900',
    marginBottom: 8,
  },
  bannerText: {
    color: colors.textSecondary,
    lineHeight: 20,
  },
  card: {
    backgroundColor: colors.surface,
    borderWidth: 1,
    borderColor: colors.border,
    borderRadius: 24,
    padding: 20,
    marginBottom: 14,
  },
  cardLabel: {
    color: colors.textMuted,
    fontSize: 11,
    letterSpacing: 1.4,
    fontWeight: '800',
    marginBottom: 8,
  },
  gasLevel: {
    color: colors.textPrimary,
    fontSize: 44,
    fontWeight: '900',
    marginBottom: 10,
  },
  info: {
    color: colors.textSecondary,
    fontSize: 16,
    lineHeight: 24,
  },
  statsRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    marginBottom: 16,
  },
  statBox: {
    width: '31%',
    backgroundColor: colors.surfaceLight,
    borderRadius: 18,
    padding: 14,
    borderWidth: 1,
    borderColor: colors.border,
  },
  statLabel: {
    color: colors.textMuted,
    fontSize: 11,
    letterSpacing: 1,
    marginBottom: 8,
  },
  statValue: {
    color: colors.textPrimary,
    fontSize: 18,
    fontWeight: '900',
  },
  controlCard: {
    backgroundColor: colors.surface,
    borderRadius: 24,
    borderWidth: 1,
    borderColor: colors.border,
    padding: 18,
    marginBottom: 16,
  },
  sectionTitle: {
    color: colors.textPrimary,
    fontSize: 18,
    fontWeight: '900',
    marginBottom: 8,
  },
  sectionText: {
    color: colors.textSecondary,
    lineHeight: 20,
    marginBottom: 12,
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
  callButton: {
    flexDirection: 'row',
    backgroundColor: colors.danger,
    paddingVertical: 16,
    paddingHorizontal: 32,
    borderRadius: 30,
    alignItems: 'center',
    justifyContent: 'center',
  },
  callButtonText: {
    color: colors.textPrimary,
    fontSize: 18,
    fontWeight: 'bold',
    marginLeft: 12,
  },
});
