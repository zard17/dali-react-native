import React from 'react';
import { View, Text, Image } from 'react-native';

// Simple demo app - showcases View, Text, and Image components
const App = () => {
    return (
        <>
            <Text style={{ position: 'absolute', top: 50, left: 50, fontSize: 32 }}>
                Hello from React Native!
            </Text>
            <Text style={{ position: 'absolute', top: 100, left: 50, fontSize: 18 }}>
                Powered by DALi + Fabric
            </Text>
        </>
    );
};

export default App;
