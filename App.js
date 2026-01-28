import React from 'react';
import { View, Text } from 'react-native';

// Flexbox layout test
const App = () => {
    return (
        <View style={{ flex: 1, backgroundColor: '#222', padding: 20 }}>
            {/* Header */}
            <View style={{ height: 60, backgroundColor: '#4a90d9', justifyContent: 'center', alignItems: 'center', marginBottom: 20 }}>
                <Text style={{ color: 'white', fontSize: 20 }}>Header</Text>
            </View>

            {/* Row of 3 boxes */}
            <View style={{ flexDirection: 'row', justifyContent: 'space-between', marginBottom: 20 }}>
                <View style={{ width: 100, height: 100, backgroundColor: '#e74c3c', justifyContent: 'center', alignItems: 'center' }}>
                    <Text style={{ color: 'white' }}>Box 1</Text>
                </View>
                <View style={{ width: 100, height: 100, backgroundColor: '#2ecc71', justifyContent: 'center', alignItems: 'center' }}>
                    <Text style={{ color: 'white' }}>Box 2</Text>
                </View>
                <View style={{ width: 100, height: 100, backgroundColor: '#3498db', justifyContent: 'center', alignItems: 'center' }}>
                    <Text style={{ color: 'white' }}>Box 3</Text>
                </View>
            </View>

            {/* Flex grow row */}
            <View style={{ flexDirection: 'row', height: 80, marginBottom: 20 }}>
                <View style={{ flex: 1, backgroundColor: '#9b59b6', justifyContent: 'center', alignItems: 'center' }}>
                    <Text style={{ color: 'white' }}>Flex 1</Text>
                </View>
                <View style={{ flex: 2, backgroundColor: '#e67e22', justifyContent: 'center', alignItems: 'center' }}>
                    <Text style={{ color: 'white' }}>Flex 2</Text>
                </View>
                <View style={{ flex: 1, backgroundColor: '#1abc9c', justifyContent: 'center', alignItems: 'center' }}>
                    <Text style={{ color: 'white' }}>Flex 1</Text>
                </View>
            </View>

            {/* Nested flex */}
            <View style={{ flex: 1, flexDirection: 'row' }}>
                <View style={{ flex: 1, backgroundColor: '#34495e', margin: 5, justifyContent: 'center', alignItems: 'center' }}>
                    <Text style={{ color: 'white' }}>Left</Text>
                </View>
                <View style={{ flex: 2, backgroundColor: '#7f8c8d', margin: 5 }}>
                    <View style={{ flex: 1, backgroundColor: '#c0392b', margin: 5, justifyContent: 'center', alignItems: 'center' }}>
                        <Text style={{ color: 'white' }}>Top</Text>
                    </View>
                    <View style={{ flex: 1, backgroundColor: '#27ae60', margin: 5, justifyContent: 'center', alignItems: 'center' }}>
                        <Text style={{ color: 'white' }}>Bottom</Text>
                    </View>
                </View>
            </View>

            {/* Footer */}
            <View style={{ height: 40, backgroundColor: '#4a90d9', justifyContent: 'center', alignItems: 'center', marginTop: 20 }}>
                <Text style={{ color: 'white' }}>Footer</Text>
            </View>
        </View>
    );
};

export default App;
