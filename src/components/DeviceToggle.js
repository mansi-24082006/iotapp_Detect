import React, { useEffect } from 'react';
import { View, Text, Switch, StyleSheet, Pressable } from 'react-native';
import { colors } from '../theme/colors';

export const DeviceToggle = ({
  title,
  icon: Icon,
  value,
  onToggle,
  disabled,
}) => {
  return (
    <View style={[styles.container, value && styles.containerActive]}>
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
    </View>
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
