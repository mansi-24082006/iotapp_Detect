import React, { useEffect } from 'react';
import { View, Text, Switch, StyleSheet, Pressable } from 'react-native';
import { MotiView } from 'moti';
import { colors } from '../theme/colors';

export const DeviceToggle = ({ title, icon: Icon, value, onToggle, disabled }) => {
  return (
    <MotiView
      style={[styles.container, value && styles.containerActive]}
      animate={{
        scale: value ? 1.05 : 1,
        backgroundColor: value ? colors.surfaceLight : colors.surface,
        borderColor: value ? colors.primary : colors.border,
        shadowOpacity: value ? 0.8 : 0,
        rotateX: value ? '5deg' : '0deg', // 3D tilt effect when active
        rotateY: value ? '-2deg' : '0deg',
      }}
      transition={{ type: 'spring', damping: 12 }}
    >
      <View style={styles.header}>
        <Icon color={value ? colors.primary : colors.textSecondary} size={24} />
        <Text style={[styles.title, value && styles.titleActive]}>{title}</Text>
      </View>
      <Switch
        trackColor={{ false: colors.surfaceLight, true: colors.primary }}
        thumbColor={colors.textPrimary}
        ios_backgroundColor={colors.surfaceLight}
        onValueChange={onToggle}
        value={value}
        disabled={disabled}
      />
    </MotiView>
  );
};

const styles = StyleSheet.create({
  container: {
    padding: 20,
    borderRadius: 20,
    backgroundColor: colors.surface,
    borderWidth: 1,
    borderColor: colors.border,
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'space-between',
    marginBottom: 16,
    // Base 3D properties
    width: '100%',
    shadowColor: colors.primaryGlow,
    shadowOffset: { width: 0, height: 10 },
    shadowRadius: 15,
    elevation: 10,
  },
  containerActive: {
    borderColor: colors.primary,
  },
  header: {
    flexDirection: 'row',
    alignItems: 'center',
  },
  title: {
    color: colors.textSecondary,
    fontSize: 18,
    fontWeight: '600',
    marginLeft: 12,
  },
  titleActive: {
    color: colors.textPrimary,
  },
});
