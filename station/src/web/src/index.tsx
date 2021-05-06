import React from 'react';
import ReactDOM from 'react-dom';
import { ReactFlowProvider } from 'react-flow-renderer';

import App from './App';

ReactDOM.render(
  <React.StrictMode>
    <ReactFlowProvider>
      <App/>
    </ReactFlowProvider>
  </React.StrictMode>,
  document.getElementById('root')
);
