using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace SyncBlink.Desktop
{
    public class SyncBlinkApi : IDisposable
    {
        private bool _disposed = false;

        private readonly List<OnFrequencyFunction> _freqCallBacks = new List<OnFrequencyFunction>();
        private readonly List<OnMessageFunction> _messageCallBacks = new List<OnMessageFunction>();
        private readonly List<OnConnectionFunction> _connectionCallBacks = new List<OnConnectionFunction>();
        private readonly IntPtr _apiPtr;

        public SyncBlinkApi(long analyzerId, string analyzerName, bool setAnalyzerOnConnect)
        {
            _apiPtr = Bindings.Init(analyzerId, analyzerName, setAnalyzerOnConnect);
        }

        public void TryConnect()
        {
            Bindings.TryConnect(_apiPtr);
        }

        public void OnFrequencyUpdate(OnFrequencyFunction func)
        {
            _freqCallBacks.Add(func);
            Bindings.OnFreqUpdate(_apiPtr, func);
        }

        public void OnMessage(OnMessageFunction func)
        {
            _messageCallBacks.Add(func);
            Bindings.OnMessage(_apiPtr, func);
        }

        public void OnConnectionChange(OnConnectionFunction func)
        {
            _connectionCallBacks.Add(func);
            Bindings.OnConnectionChange(_apiPtr, func);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!_disposed)
            {
                _freqCallBacks.Clear();
                _messageCallBacks.Clear();
                _connectionCallBacks.Clear();
                _disposed = true;
            }
        }
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void OnFrequencyFunction(
        byte volume,
        ushort dominantFrequency);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void OnMessageFunction(
        [MarshalAs(UnmanagedType.LPStr)] string message,
        bool isError);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void OnConnectionFunction(
        [MarshalAs(UnmanagedType.LPStr)] string hubIp, 
        bool isConnected);

    internal static class Bindings
    {
        private const string DllFile = "syncblinkapi";

        [DllImport(DllFile, EntryPoint = "syncblink_api_init", CallingConvention = CallingConvention.Cdecl)]
        internal extern static IntPtr Init(long analyzerId, [MarshalAs(UnmanagedType.LPStr)] string analyzerName, bool setAnalyzerOnConnect);

        [DllImport(DllFile, EntryPoint = "syncblink_api_tryconnect", CallingConvention = CallingConvention.Cdecl)]
        internal extern static void TryConnect(IntPtr syncBlinkApi);

        [DllImport(DllFile, EntryPoint = "syncblink_api_on_freq", CallingConvention = CallingConvention.Cdecl)]
        internal extern static void OnFreqUpdate(IntPtr syncBlinkApi, OnFrequencyFunction onFrequencyFunction);

        [DllImport(DllFile, EntryPoint = "syncblink_api_on_message", CallingConvention = CallingConvention.Cdecl)]
        internal extern static void OnMessage(IntPtr syncBlinkApi, OnMessageFunction onMessageFunction);

        [DllImport(DllFile, EntryPoint = "syncblink_api_on_connection", CallingConvention = CallingConvention.Cdecl)]
        internal extern static void OnConnectionChange(IntPtr syncBlinkApi, OnConnectionFunction onConnectionFunction);
    }
}
