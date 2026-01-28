// Test App for TOP_LEFT rendering verification
// This creates views across the entire screen to test coordinate system

import React from 'react';
import { View, Text } from 'react-native';

const App = () => {
    // Create a grid of colored squares across the entire screen
    // If TOP_LEFT works correctly, all squares should be visible
    const squares = [];
    const squareSize = 80;
    const gap = 10;
    const cols = 10;
    const rows = 6;

    for (let row = 0; row < rows; row++) {
        for (let col = 0; col < cols; col++) {
            const i = row * cols + col;
            const x = col * (squareSize + gap) + 50;
            const y = row * (squareSize + gap) + 50;

            // Color gradient: red to blue horizontally, green vertically
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
                        width: squareSize,
                        height: squareSize,
                        backgroundColor: `rgb(${r}, ${g}, ${b})`,
                        justifyContent: 'center',
                        alignItems: 'center',
                    }}
                >
                    <Text style={{ color: 'white', fontSize: 10 }}>
                        {`${x},${y}`}
                    </Text>
                </View>
            );
        }
    }

    return (
        <View style={{ flex: 1, backgroundColor: '#333' }}>
            <Text style={{ color: 'white', padding: 10, fontSize: 16 }}>
                TOP_LEFT Test - All squares should be visible
            </Text>
            {squares}
        </View>
    );
};

export default App;
