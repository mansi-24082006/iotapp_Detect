import React from 'react';
import { StyleSheet, View } from 'react-native';
import { MotiView, MotiText } from 'moti';
import Svg, { Circle } from 'react-native-svg';
import { colors } from '../theme/colors';

const RADIUS = 100;
const CIRCUMFERENCE = 2 * Math.PI * RADIUS;

export const AnimatedGauge = ({ value, max = 2000, status }) => {
  const isDanger = status === 'DANGER';
  const progress = Math.min(value / max, 1);
  const strokeDashoffset = CIRCUMFERENCE - progress * CIRCUMFERENCE;
  const activeColor = isDanger ? colors.danger : colors.primary;
  const activeGlow = isDanger ? colors.dangerGlow : colors.primaryGlow;

  return (
    <View style={styles.container}>
      <MotiView
        animate={{
          shadowColor: activeColor,
          shadowOpacity: isDanger ? 0.8 : 0.5,
          scale: isDanger ? [1, 1.05, 1] : 1, // Pulse effect in danger
        }}
        transition={{
          loop: isDanger,
          type: 'timing',
          duration: 1000,
        }}
        style={styles.gaugeWrapper}
      >
        <Svg width={240} height={240} viewBox="0 0 240 240">
          <Circle
            cx="120"
            cy="120"
            r={RADIUS}
            stroke={colors.surfaceLight}
            strokeWidth="15"
            fill="transparent"
          />
          <Circle
            cx="120"
            cy="120"
            r={RADIUS}
            stroke={activeColor}
            strokeWidth="15"
            strokeDasharray={CIRCUMFERENCE}
            strokeDashoffset={strokeDashoffset}
            strokeLinecap="round"
            fill="transparent"
            transform="rotate(-90 120 120)"
          />
        </Svg>
        <View style={styles.centerText}>
          <MotiText 
            style={[styles.valueText, { color: activeColor }]}
            animate={{ scale: isDanger ? [1, 1.1, 1] : 1 }}
            transition={{ loop: isDanger, duration: 1000 }}
          >
            {value}
          </MotiText>
          <MotiText style={styles.label}>PPM</MotiText>
        </View>
      </MotiView>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    alignItems: 'center',
    justifyContent: 'center',
    paddingVertical: 40,
  },
  gaugeWrapper: {
    shadowOffset: { width: 0, height: 0 },
    shadowRadius: 30,
    elevation: 20,
    position: 'relative',
    alignItems: 'center',
    justifyContent: 'center',
  },
  centerText: {
    position: 'absolute',
    alignItems: 'center',
    justifyContent: 'center',
  },
  valueText: {
    fontSize: 48,
    fontWeight: 'bold',
  },
  label: {
    fontSize: 16,
    color: colors.textSecondary,
    marginTop: 4,
  },
});
