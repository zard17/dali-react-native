import React from 'react';
import { AppRegistry, View, Text, Image } from 'react-native';

const App = () => {
    console.log('App: Rendering...');
    React.useEffect(() => {
        console.log('App: Mounted!');
    }, []);
    return (
        <View style={{ flex: 1, backgroundColor: 'white', justifyContent: 'center', alignItems: 'center' }}>
            <Image
                source={{ uri: '/Users/youngsus/tizen/dali-react-native/screenshot.png' }}
                style={{ width: 200, height: 200, marginBottom: 20 }}
            />
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
