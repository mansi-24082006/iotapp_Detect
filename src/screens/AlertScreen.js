import React from 'react';
import { View, Text, StyleSheet, Pressable } from 'react-native';
import { MotiView } from 'moti';
import { useIoTStore } from '../store/useIoTStore';
import { colors } from '../theme/colors';
import { AlertOctagon, PhoneCall } from 'lucide-react-native';
import LinearGradient from 'react-native-linear-gradient';

export const AlertScreen = () => {
  const { device, settings } = useIoTStore();
  const gasLevel = device.gasLevel;

  return (
    <LinearGradient
      colors={[colors.dangerBackground, colors.background]}
      style={styles.container}
    >
      <MotiView
        from={{ scale: 0.8, opacity: 0 }}
        animate={{ scale: [1, 1.1, 1], opacity: 1 }}
        transition={{ type: 'timing', duration: 1000, loop: true }}
      >
        <AlertOctagon size={120} color={colors.danger} />
      </MotiView>

      <Text style={styles.title}>EMERGENCY</Text>
      <Text style={styles.subtitle}>HIGH GAS LEAK DETECTED</Text>
      
      <MotiView 
        style={styles.card}
        animate={{
          rotateY: ['-5deg', '5deg', '-5deg'], // Aggressive 3D shake
          rotateX: ['5deg', '-5deg', '5deg'],
        }}
        transition={{ loop: true, duration: 400 }}
      >
        <Text style={styles.gasLevel}>{gasLevel} PPM</Text>
        <Text style={styles.info}>Evacuate immediately. Exhaust fan and buzzer have been automatically activated.</Text>
      </MotiView>

      {settings.emergencyCallEnabled && (
        <MotiView
          from={{ translateY: 50, opacity: 0 }}
          animate={{ translateY: 0, opacity: 1 }}
          transition={{ delay: 500 }}
        >
          <Pressable style={styles.callButton}>
            <PhoneCall color={colors.textPrimary} size={24} />
            <Text style={styles.callButtonText}>Call {settings.emergencyNumber}</Text>
          </Pressable>
        </MotiView>
      )}
    </LinearGradient>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    padding: 24,
  },
  title: {
    color: colors.danger,
    fontSize: 42,
    fontWeight: '900',
    marginTop: 20,
    letterSpacing: 2,
    textShadowColor: colors.danger,
    textShadowOffset: { width: 0, height: 0 },
    textShadowRadius: 20,
  },
  subtitle: {
    color: colors.textPrimary,
    fontSize: 18,
    fontWeight: 'bold',
    marginTop: 10,
    marginBottom: 40,
  },
  card: {
    backgroundColor: 'rgba(255,51,102,0.1)',
    borderWidth: 2,
    borderColor: colors.danger,
    borderRadius: 20,
    padding: 30,
    alignItems: 'center',
    width: '100%',
    marginBottom: 40,
  },
  gasLevel: {
    color: colors.textPrimary,
    fontSize: 48,
    fontWeight: 'black',
    marginBottom: 10,
  },
  info: {
    color: colors.textSecondary,
    fontSize: 16,
    textAlign: 'center',
    lineHeight: 24,
  },
  callButton: {
    flexDirection: 'row',
    backgroundColor: colors.danger,
    paddingVertical: 16,
    paddingHorizontal: 32,
    borderRadius: 30,
    alignItems: 'center',
  },
  callButtonText: {
    color: colors.textPrimary,
    fontSize: 18,
    fontWeight: 'bold',
    marginLeft: 12,
  },
});
