import React from 'react';
import { AppRegistry, View, Text } from 'react-native';

const App = () => {
    return (
        <View style={{ flex: 1, backgroundColor: 'white', justifyContent: 'center', alignItems: 'center' }}>
            <Text style={{ fontSize: 32, fontWeight: 'bold', color: 'blue' }}>
                Hello from React Native!
            </Text>
            <Text style={{ fontSize: 18, marginTop: 20, color: 'gray' }}>
                Powered by Dali + Fabric
            </Text>
        </View>
    );
};

AppRegistry.registerComponent('DaliRNApp', () => App);
global.AppRegistry = AppRegistry;
