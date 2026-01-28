import React from 'react';
import { View } from 'react-native';

const App = () => {
    const views = [];
    const VIEW_COUNT = 1000;
    const cols = 20;
    const w = 20;
    const h = 20;

    for (let i = 0; i < VIEW_COUNT; i++) {
        const top = Math.floor(i / cols) * h;
        const left = (i % cols) * w;
        views.push(
            <View
                key={i}
                style={{
                    position: 'absolute',
                    top: top,
                    left: left,
                    width: w,
                    height: h,
                    backgroundColor: `rgba(${Math.round((i % 10) / 10 * 255)}, ${Math.round((Math.floor(i / 10) % 10) / 10 * 255)}, 128, 1)`
                }}
            />
        );
    }

    return (
        <View style={{ flex: 1, backgroundColor: 'white' }}>
            {views}
        </View>
    );
};

export default App;
