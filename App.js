import React from 'react';
import { View, Text } from 'react-native';

// Test app to debug parent-child hierarchy
// Using collapsable={false} to prevent React Native view flattening
const App = () => {
    return (
        <View
            style={{ position: 'absolute', top: 0, left: 0, width: 800, height: 600, backgroundColor: '#333' }}
            collapsable={false}
        >
            <View
                style={{ position: 'absolute', top: 50, left: 50, width: 300, height: 200, backgroundColor: '#4CAF50' }}
                collapsable={false}
            >
                <Text style={{ position: 'absolute', top: 80, left: 100, fontSize: 20, color: 'white' }}>
                    Nested Text
                </Text>
            </View>
        </View>
    );
};

export default App;
