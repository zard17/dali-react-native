import React from 'react';
import { View, Text, Image } from 'react-native';

const App = () => {
    const colors = ["red", "green", "blue", "yellow", "cyan", "magenta", "orange", "purple", "black", "gray"];
    return (
        <View style={{ flex: 1, flexDirection: 'row', flexWrap: 'wrap', backgroundColor: 'white' }}>
            {Array.from({ length: 50 }).map((_, i) => (
                <View key={i} style={{ width: 100, height: 100, backgroundColor: colors[i % 10], margin: 5 }} />
            ))}
            <Text style={{ position: 'absolute', bottom: 50, left: 0, right: 0, textAlign: 'center' }}>Grid Layout Demo</Text>
        </View>
    );
};
