import React from 'react';
import { View, Text } from 'react-native';

// Test various positions to find the negative threshold
// Window: 1920x1080, Center: (960, 540)
// DALi position = RN position - (960, 540)
const App = () => {
    return (
        <View style={{ flex: 1, backgroundColor: 'white' }}>
            {/* RN(460, 40) → DALi(-500, -500) - testing threshold */}
            <View style={{ position: 'absolute', left: 460, top: 40, width: 50, height: 50, backgroundColor: 'red' }} />

            {/* RN(560, 140) → DALi(-400, -400) */}
            <View style={{ position: 'absolute', left: 560, top: 140, width: 50, height: 50, backgroundColor: 'orange' }} />

            {/* RN(660, 240) → DALi(-300, -300) */}
            <View style={{ position: 'absolute', left: 660, top: 240, width: 50, height: 50, backgroundColor: 'yellow' }} />

            {/* RN(760, 340) → DALi(-200, -200) */}
            <View style={{ position: 'absolute', left: 760, top: 340, width: 50, height: 50, backgroundColor: 'green' }} />

            {/* RN(860, 440) → DALi(-100, -100) */}
            <View style={{ position: 'absolute', left: 860, top: 440, width: 50, height: 50, backgroundColor: 'cyan' }} />

            {/* RN(910, 490) → DALi(-50, -50) - this one worked */}
            <View style={{ position: 'absolute', left: 910, top: 490, width: 50, height: 50, backgroundColor: 'blue' }} />

            {/* RN(960, 540) → DALi(0, 0) - center */}
            <View style={{ position: 'absolute', left: 960, top: 540, width: 50, height: 50, backgroundColor: 'purple' }} />
        </View>
    );
};

export default App;
