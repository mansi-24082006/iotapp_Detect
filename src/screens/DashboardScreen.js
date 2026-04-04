import React from 'react';
import { View, Text, StyleSheet, ScrollView, Pressable } from 'react-native';
import { useNavigation } from '@react-navigation/native';
import {
  Fan,
  Bell,
  Power,
  Settings as SettingsIcon,
} from 'lucide-react-native';
import { useIoTStore } from '../store/useIoTStore';
import { colors } from '../theme/colors';
import { AnimatedGauge } from '../components/AnimatedGauge';
import { DeviceToggle } from '../components/DeviceToggle';
import LinearGradient from 'react-native-linear-gradient';

export const DashboardScreen = () => {
  const { device, updateDeviceToggle, isConnected } = useIoTStore();
  const navigation = useNavigation();

  return (
    <LinearGradient
      colors={[colors.surface, colors.background]}
      style={styles.container}
    >
      <ScrollView contentContainerStyle={styles.scroll}>
        <View style={styles.header}>
          <View>
            <Text style={styles.title}>IoT Gas System</Text>
            <View
              style={[
                styles.statusBadge,
                {
                  backgroundColor: isConnected ? colors.success : colors.danger,
                },
              ]}
            >
              <Text style={styles.statusText}>
                {isConnected ? 'ONLINE' : 'OFFLINE'}
              </Text>
            </View>
          </View>
          <Pressable onPress={() => navigation.navigate('Settings')}>
            <SettingsIcon color={colors.textSecondary} size={28} />
          </Pressable>
        </View>

        <View style={styles.card3d}>
          <AnimatedGauge value={device.gasLevel} status={device.status} />
          <View style={styles.indicatorContainer}>
            <Text style={styles.indicatorLabel}>System Status:</Text>
            <Text
              style={[
                styles.indicatorValue,
                {
                  color:
                    device.status === 'SAFE' ? colors.success : colors.danger,
                },
              ]}
            >
              {device.status}
            </Text>
          </View>
        </View>

        <View>
          <Text style={styles.sectionTitle}>Manual Controls</Text>
          <DeviceToggle
            title="Exhaust Fan"
            icon={Fan}
            value={device.fan}
            onToggle={val => updateDeviceToggle('fan', val)}
            disabled={!isConnected || device.status === 'DANGER'} // Auto locked in danger
          />
          <DeviceToggle
            title="Buzzer Alarm"
            icon={Bell}
            value={device.buzzer}
            onToggle={val => updateDeviceToggle('buzzer', val)}
            disabled={!isConnected || device.status === 'DANGER'}
          />
          <DeviceToggle
            title="Gas Valve (Servo)"
            icon={Power}
            value={device.servo}
            onToggle={val => updateDeviceToggle('servo', val)}
            disabled={!isConnected || device.status === 'DANGER'}
          />
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
    padding: 24,
    paddingTop: 60,
  },
  header: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: 40,
  },
  title: {
    color: colors.textPrimary,
    fontSize: 28,
    fontWeight: 'bold',
  },
  statusBadge: {
    paddingHorizontal: 12,
    paddingVertical: 6,
    borderRadius: 20,
  },
  statusText: {
    color: colors.background,
    fontWeight: 'bold',
    fontSize: 12,
  },
  card3d: {
    backgroundColor: colors.surfaceLight,
    borderRadius: 30,
    padding: 20,
    marginBottom: 40,
    // 3D layering
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
    marginTop: 20,
    paddingTop: 20,
    borderTopWidth: 1,
    borderTopColor: colors.border,
  },
  indicatorLabel: {
    color: colors.textSecondary,
    fontSize: 18,
    marginRight: 10,
  },
  indicatorValue: {
    fontSize: 20,
    fontWeight: '900',
    letterSpacing: 1,
  },
  sectionTitle: {
    color: colors.textSecondary,
    fontSize: 16,
    fontWeight: '600',
    marginBottom: 16,
    marginLeft: 8,
  },
});
