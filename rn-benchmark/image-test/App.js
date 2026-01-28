import React from 'react';
import { View, Image, Dimensions } from 'react-native';

const App = () => {
    const images = [];
    const IMAGE_COUNT = 100;
    const cols = 10;
    const w = 50;
    const h = 50;

    // Grid dimensions
    const gridWidth = cols * w;
    const gridHeight = (IMAGE_COUNT / cols) * h;

    // Get window dimensions and calculate center offset
    const { width: windowWidth, height: windowHeight } = Dimensions.get('window');
    const offsetX = (windowWidth - gridWidth) / 2;
    const offsetY = (windowHeight - gridHeight) / 2;

    // Absolute path to assets directory
    const ASSET_PATH = '/Users/youngsus/tizen/dali-react-native/assets';

    for (let i = 0; i < IMAGE_COUNT; i++) {
        const top = offsetY + Math.floor(i / cols) * h;
        const left = offsetX + (i % cols) * w;
        images.push(
            <Image
                key={i}
                source={{ uri: `${ASSET_PATH}/image_${i % 10}.png` }}
                style={{
                    position: 'absolute',
                    top: top,
                    left: left,
                    width: w,
                    height: h,
                }}
            />
        );
    }

    return (
        <View style={{ flex: 1, backgroundColor: 'white' }}>
            {images}
        </View>
    );
};

export default App;
