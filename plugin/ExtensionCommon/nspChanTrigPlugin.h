/* =STS=> nspChanTrigPlugin.h[5198].aa01   submit   SMID:2 */
//////////////////////////////////////////////////////////////////////////////
///
/// @file nspChanTrigPlugin.h
/// @author  Ehsan Azar
/// @copyright (c) Copyright Blackrock Microsystems
/// @brief
///    Channel trigger plugin API
/// @details
///  Communicate with the plugin container to:<br>
///   grab data, do custom processing, and trigger an output channel.<br>
///  Communicate to/from CereLink through logging/comment commands.<br>
///  Contiguous access to data, makes it easier for vector processings.
///
/// @date 3/4/13 4:24p
///
////////////////////////////////////////////////////////////////////////////////
///
/// @internal
/// @note This file is distributed to the customer.<br>
///        do not include firmware headers.<br>
///       When adding new data:<br>
///        Provide contiguous access to data, to make it easier for vector processings.<br>
/// @note
///       Since this is an API, keep the rich set of documents in the header file.<br>
/// @endinternal
///
/// @addtogroup PluginAPI
/// @{
///

#ifndef NSPCHANTRIGPLUGIN_H_INCLUDED       // include guards
#define NSPCHANTRIGPLUGIN_H_INCLUDED

#include "nspPlugin.h"

#define CBEXT_FRONTEND_COUNT     256    ///< Number of front-end channels
#define CBEXT_ANALOGINPUT_COUNT  16     ///< Number of analog output channels
#define CBEXT_INPUT_COUNT  (CBEXT_FRONTEND_COUNT + CBEXT_ANALOGINPUT_COUNT)     ///< Total number of sampled input channels
#define CBEXT_DIGITALINPUT_COUNT 1      ///< Number of digital input channels
#define CBEXT_ANALOGOUTPUT_COUNT 6      ///< Number of analog output channels

#ifndef cbMAX_PNTS
#define cbMAX_PNTS 128
#endif

/**
 * Channel trigger active settings (provided by firmware to the extension)
 * @see cbExtMainLoop
 * @brief Plugin settings provided by firmware.<br>
 * maximum number of buffered samples from one input channel (front-end, analog input and digital).<br>
 * maximum number of buffered spikes from one input channel with spike capbility (front-end and analog input).<br>
 * maximum number of triggers generated by extension buffered for firmware.
 * maximum number of samples generated by extension buffered for firmware.
 * @warning Extension should rely on these settings instead of any requested values.
 * @note Number of samples will affect the memory quota of the extension.<br>
 * Different buffer lengths can be requested by exporting symbols with the same name and type.<br>
 * Buffer lengths are determined before plugin main loop and cannot be changed.
 */
typedef struct {
    uint32_t nSamplesBufferLen;    ///< circular samples buffer length, from firmware to extension (determines assigned memory assuming full sample-rate)
    uint32_t nSpikesBufferLen;     ///< circular spike buffer length, from firmware to extension (determines assigned memory assuming full sample-rate)
    uint32_t nTrigsBufferLen;      ///< circular trigger buffer length, from extension to firmware
    uint32_t nOutputBufferLen;     ///< circular analog output buffer length, from extension to firmware
} cbExtChanTrigSettings;

/**
 * Channel trigger plugin information (provided by extension to firmware)
 * divider values are applied at CBEXT_MAX_SAMPLERATE, 0 means disabled, 1 means full, 2 means half, ...
 * @note samples buffers include frontend, analog and digital input.<br>
 * @see cbExtChanTrigSetup()
 * @brief Trigger plugin information provided by extension
 */
typedef struct {
    uint32_t nWarnSamplesThreshold;     ///< Threshold (in percent) over which samples buffer should be emptied, or will result in warning (0 means no warning)
    uint32_t nWarnSpikesThreshold;      ///< Threshold (in percent) over which spikes buffer should be emptied, or will result in warning (0 means no warning)
    uint8_t reserved;                   ///< Reserved for future use (must set to 0)
    uint8_t nDividerFrontend;           ///< Sample rate divider for front-end channels (0 means disable and will not consume memory)
    uint8_t nDividerAnalogInput;        ///< Sample rate divider for analog input channels (0 means disable and will not consume memory)
    uint8_t nDividerDigitalInput;       ///< Sample rate divider for digital input channels (0 means disable and will not consume memory)
    uint8_t nSpikes;                    ///< Capture spikes (0 means disable channel and will not consume memory)
} cbExtChanTrigInfo;

/**
 * Number of samples for channels
 * @brief Channel samples count
 * @see cbExtChanTrigConsumeSamples(), cbExtChanTrigSamples
 */
typedef struct {
    uint32_t nCountFrontend;              ///< Number of front-end samples in buffer
    uint32_t nCountAnalogInput;           ///< Number of analog-input samples in buffer
    uint32_t nCountDigitalInput;          ///< Number of digital-input samples in buffer
} cbExtChanTrigSamplesCount;

/**
 * Input samples from channels
 * @warning Make sure the buffers are pre-allocated to hold requested number of samples.<br>
 * @brief Channel samples
 * @see cbExtChanTrigGetSamples()
 */
typedef struct {
    /**
     * @brief samples count
     */
    cbExtChanTrigSamplesCount isCount;
    /**
     * @brief Front-end samples
     */
    struct _FrontendSamples {
        uint32_t nProctime;                   ///< First sample proctime
        float * pfData[CBEXT_FRONTEND_COUNT]; ///< samples of each channel (#pfData[0] to #pfData[#isCount (of this channel) - 1])
    } isFrontend; ///< sample from the front-end channel queue
    /**
     * @brief analog input samples
     */
    struct _AnalogInputSamples {
        uint32_t nProctime;                            ///< First sample proctime
        float * pfData[CBEXT_ANALOGINPUT_COUNT];       ///< samples of each channel (#pfData[0] to #pfData[#isCount (of this channel) - 1])
    } isAnalogInput; ///< sample from analog-input channel queue
    /**
     * @brief digital input samples
     */
    struct _DigitalInputSamples {
        uint32_t nProctime;                             ///< First sample proctime
        uint16_t * pnData[CBEXT_DIGITALINPUT_COUNT];    ///< samples of each channel (#pnData[0] to #pnData[#isCount (of this channel) - 1])
    } isDigitalInput; ///< sample from digital input channel queue
} cbExtChanTrigSamples;

/**
 * Number of samples for output channels
 * @brief Channel samples count
 * @see cbExtChanTrigSendSamples()
 */
typedef struct
{
    uint32_t nCountAnalogOutput;      ///< Number of analog-output samples in buffer
} cbExtChanTrigOutputSamplesCount;

/**
 * Samples from the extension for the Analog Output ports
 * @warning Make sure the buffers are pre-allocated to hold requested number of samples.<br>
 * @brief Analog Output samples
 * @see cbExtChanTrigSendSamples()
 */
typedef struct
{
    /**
     * @brief samples count
     */
    cbExtChanTrigOutputSamplesCount isCount;
    /**
     * @brief analog output samples
     */
    struct _AnalogOutputSamples
    {
        uint32_t * pnProctime;                         ///< Proctime when samples should go out (defaults to 0 or the time of the associated input sample)
        int16_t * pnData[CBEXT_ANALOGOUTPUT_COUNT];   ///< samples for each channel (#pnData[0] to #pnData[#isCount (of this channel) - 1]).
    } isAnalogOutput; ///< sample from analog-input channel queue
} cbExtChanTrigOutputSamples;

/// Number of spikes
/// @brief Channel samples count
/// @see cbExtChanTrigGetSpikes()
typedef struct
{
    uint32_t nCountSpikes;      ///< Number of spikes in buffer
} cbExtChanTrigSpikesCount;

/// Spike elements
/// @brief Spike element structure
/// @see cbExtChanTrigSpikes()
typedef struct {
    uint32_t nTime;
    uint16_t nChan;
    uint16_t nUnit;
    uint16_t nWaveform[cbMAX_PNTS];
} cbExtSpikeElement;

/// Spike samples
/// @brief Channel spike samples
/// @see cbExtChanTrigGetSpikes()
typedef struct {
    cbExtChanTrigSpikesCount isCount;

    cbExtSpikeElement *isSpike;

} cbExtChanTrigSpikes;

/**
 * Every channel trigger plugin (specified in #cbExtIntent by #cbExtSetup) must also implement this method.
 * @brief Get channel trigger plugin information from plugin
 *
 * @warning Plugin is not setup when this is called, plugin API commands must not be called from within this function.<br>
 * Use #cbExtMainLoop to imlement plugin code.
 * @note This function should not block for long.<br>
 * Providing this function satisfies CBEXT_INTENT_CHANTRIG intent.
 *
 * @param[out]  info Channel trigger plugin information requested by firmware, provided by plugin
 * @return API function return value ::cbExtResult<br>
 * If an error is returned plugin will not run
 */
CBEXTAPI    cbExtResult cbExtChanTrigSetup(cbExtChanTrigInfo * info);

/**
 * If plugin intent specifies channel trigger, this will return the settings.
 * @brief Get channel trigger plugin settings
 * @param[out] settings Channel trigger plugin settings (NULL on error)
 * @return API function return value ::cbExtResult
 */
cbExtResult cbExtChanTrigGetSettings(cbExtChanTrigSettings * settings);


/**
 * Get the latest samples in the circular buffer.<br>
 * @brief Get channel samples
 *
 * @warning
 * If this is not called frequently enough for requested sample rate (specified by #cbExtChanTrigInfo), there might be some dropped samples.<br>
 * @note
 * Each call to this function retrieves *up to* requested number of samples.<br>
 * If buffer is empty number of returned samples will be set to zero.<br>
 * There will be no time discontinuity in the returned data, all samples are equally spaced from each other.<br>
 * Dropped sample can be detected knowing the sample rate, and the starting time of each array.<br>
 * If any sample is dropped, should call this function again to resume after the dropped sample.<br>
 *
 * @warning Buffers must pre-allocated for (at least) the requested number of samples in #cbExtChanTrigSamples::isCount.<br>
 *
 * @param[in] pSampleArray pre-allocated array of samples.<br>
 * Buffer size should be given in #cbExtChanTrigSamples::isCount
 * @param[out] pSampleArray array of samples in the queue.<br>
 * #cbExtChanTrigSamples::isCount will have the valid number of samples in returned buffer.
 * @return API function return value ::cbExtResult
 */
cbExtResult cbExtChanTrigGetSamples(cbExtChanTrigSamples * pSampleArray);

/**
 * Gets number of queued samples for each channel
 * @brief Get number of available samples
 * @param[out] isCount number of samples in the queue for each channel
 * @return API function return value ::cbExtResult
 */
cbExtResult cbExtChanTrigGetSamplesCount(cbExtChanTrigSamplesCount * isCount);

/**
 * Consume maximum-of nCount samples from the circular buffer for each given channel
 * @brief Consume given number of samples
 * @param[in] isMaxConsume maximum number of samples to consume for each channel
 * @return API function return value ::cbExtResult
 */
cbExtResult cbExtChanTrigConsumeSamples(cbExtChanTrigSamplesCount isMaxConsume);

/**
 * Generate analog output trigger for given index.<br>
 * if user has set the trigger of given index to 'extension', the loaded analog wave will be generated.
 * @brief trigger analog output channel
 * @param[in] nChannel analog output channel index (starting at 0)
 * @param[in] nTriggerIndex trigger index to activate
 * @return API function return value ::cbExtResult
 */
cbExtResult cbExtChanTrigAnalogOutput(uint16_t nChannel, uint8_t nTriggerIndex);

/**
 * Generate digital output trigger for given index.<br>
 *  if user has set the trigger of given index to 'extension', the loaded digital wave will be generated.
 * @brief trigger digital output channel
 * @param[in] nChannel digital output channel number (bit number starting at 0)<br>
 * Each port has determined number of bits (bits 0-15 belong to the first digital output port)
 * @param[in] nTriggerIndex trigger index to activate
 * @return API function return value ::cbExtResult
 */
cbExtResult cbExtChanTrigDigitalOutput(uint16_t nChannel, uint8_t nTriggerIndex);

/**
 * Send samples to analog output immediately <br>
 * if the user has set the analog output channel to 'extension', the loaded samples will be sent out.
 * @note samples should be given at a 30KS/s basis
 * @brief send samples to analog output channel
 * @param[in] pSampleArray array of samples.<br>
 * Number of samples in the array should be given in #cbExtChanTrigOutputSamplesCount::isCount
 * @param[in] bSync indicates whether the firmware should send samples to analog output immediately (there is
 * a possibility of a fragmented signal in this instance) or synchronize them with the input signal they're based
 * on which will give a more consistent signal.
 * @note bSync only works for data from the firmware modified by a plugin. Plugin generated signals will always be sent immediately.
 * @return API function return value ::cbExtResult
 */
cbExtResult cbExtChanTrigSendSamples(cbExtChanTrigOutputSamples * pSampleArray, BOOL bSync);

/// Get the spikes in the circular buffer.<br>
/// @brief Get spike samples
///
/// @warning
/// If this is not called frequently enough for spike extraction rate, there might be some dropped spikes.<br>
/// @note
/// Each call to this function retrieves *up to* requested number of spikes.<br>
/// If buffer is empty, the number of returned samples will be set to zero.<br>
/// Dropped spikes cannot be detected.<br>
///
/// @warning Buffers must pre-allocated for (at least) the requested number of samples in #cbExtChanTrigSpikes::isCount.<br>
///
/// @param[in] pSpikeArray pre-allocated array of samples.<br>
/// Buffer size should be given in #cbExtChanTrigSpikes::isCount
/// @param[out] pSpikeArray array of samples in the queue.<br>
/// #cbExtChanTrigSpikes::isCount will have the valid number of spikes in returned buffer.
/// @return API function return value ::cbExtResult
cbExtResult cbExtChanTrigGetSpikes(cbExtChanTrigSpikes * pSpikeArray);
/** @} */ // end of PluginAPI group
#endif /* NSPCHANTRIGPLUGIN_H_INCLUDED */