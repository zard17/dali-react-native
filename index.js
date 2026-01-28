import React from 'react';
import { AppRegistry, View, Text, Image } from 'react-native';

import App from './rn-benchmark/image-test/App';

AppRegistry.registerComponent('DaliRNApp', () => App);
global.AppRegistry = AppRegistry;
