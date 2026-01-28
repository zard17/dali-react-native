import React from 'react';
import { View, Text } from 'react-native';

// Full screen grid test - verify Y offset workaround
const App = () => {
    const squares = [];
    const size = 60;
    const gap = 10;
    const cols = 12;
    const rows = 8;

    for (let row = 0; row < rows; row++) {
        for (let col = 0; col < cols; col++) {
            const i = row * cols + col;
            const x = col * (size + gap) + 20;
            const y = row * (size + gap) + 20;

            // Color gradient
            const r = Math.round((col / cols) * 255);
            const g = Math.round((row / rows) * 255);
            const b = 128;

            squares.push(
                <View
                    key={i}
                    style={{
                        position: 'absolute',
                        left: x,
                        top: y,
                        width: size,
                        height: size,
                        backgroundColor: `rgb(${r}, ${g}, ${b})`,
                    }}
                />
            );
        }
    }

    return (
        <View style={{ flex: 1, backgroundColor: '#333' }}>
            {squares}
        </View>
    );
};

export default App;
