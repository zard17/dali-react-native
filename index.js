import React from 'react';
import { View, Text, Image } from 'react-native';

const App = () => {
    return (
        <View style={{ flex: 1, backgroundColor: 'cyan', justifyContent: 'center', alignItems: 'center' }}>
            <View style={{ width: 200, height: 200, backgroundColor: 'magenta' }} />
            <Text>Color Test Running</Text>
        </View>
    );
};
