using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace SyncBlink.Api
{
    public class SyncBlinkApi : IDisposable
    {
        private bool _disposed = false;
        private readonly List<OnFrequencyFunction> freqCallBacks = new List<OnFrequencyFunction>();
        private readonly IntPtr _apiPtr;

        public SyncBlinkApi(string url, long analyzerId, string analyzerName)
        {
            _apiPtr = Bindings.Init(url, analyzerId, analyzerName);
        }

        public void Start()
        {
            Bindings.Start(_apiPtr);
        }

        public void OnFrequencyUpdate(OnFrequencyFunction func)
        {
            freqCallBacks.Add(func);
            Bindings.OnFreqUpdate(_apiPtr, func);
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
                Bindings.Stop(_apiPtr);
                freqCallBacks.Clear();
                _disposed = true;
            }
        }
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void OnFrequencyFunction(
        byte volume,
        ushort dominantFrequency);

    internal static class Bindings
    {
        private const string DllFile = "syncblinkapi";

        [DllImport(DllFile, EntryPoint = "syncblink_api_init", CallingConvention = CallingConvention.Cdecl)]
        internal extern static IntPtr Init([MarshalAs(UnmanagedType.LPStr)] string url, long analyzerId, [MarshalAs(UnmanagedType.LPStr)] string analyzerName);

        [DllImport(DllFile, EntryPoint = "syncblink_api_start", CallingConvention = CallingConvention.Cdecl)]
        internal extern static IntPtr Start(IntPtr syncBlinkApi);

        [DllImport(DllFile, EntryPoint = "syncblink_api_stop", CallingConvention = CallingConvention.Cdecl)]
        internal extern static IntPtr Stop(IntPtr syncBlinkApi);

        [DllImport(DllFile, EntryPoint = "syncblink_api_on_freq", CallingConvention = CallingConvention.Cdecl)]
        internal extern static void OnFreqUpdate(IntPtr syncBlinkApi, OnFrequencyFunction onFrequencyFunction);
    }
}
