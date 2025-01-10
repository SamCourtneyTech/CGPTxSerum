#pragma once
#include <JuceHeader.h>

// Enum to manage editor display style
enum class EditorStyle { thisWindow, newWindow };

class SerumInterfaceComponent : public juce::AudioProcessor,
    public juce::Component, // Ensure it inherits from juce::Component
    private juce::ChangeListener
{
public:
    SerumInterfaceComponent()
        : juce::AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    {
        appProperties.setStorageParameters([&]
            {
                juce::PropertiesFile::Options opt;
                opt.applicationName = getName();
                opt.commonToAllUsers = false;
                opt.doNotSave = false;
                opt.filenameSuffix = ".props";
                opt.ignoreCaseOfKeyNames = false;
                opt.storageFormat = juce::PropertiesFile::StorageFormat::storeAsXML;
                opt.osxLibrarySubFolder = "Application Support";
                return opt;
            }());

        pluginFormatManager.addDefaultFormats();

        if (auto savedPluginList = appProperties.getUserSettings()->getXmlValue("pluginList"))
            pluginList.recreateFromXml(*savedPluginList);

        juce::MessageManagerLock lock;
        pluginList.addChangeListener(this);

        setSize(800, 600);
    }

    ~SerumInterfaceComponent() override {}

    //==============================================================================
    // AudioProcessor overrides

    bool isBusesLayoutSupported(const BusesLayout& layouts) const final
    {
        const auto& mainOutput = layouts.getMainOutputChannelSet();
        const auto& mainInput = layouts.getMainInputChannelSet();

        if (!mainInput.isDisabled() && mainInput != mainOutput)
            return false;

        if (mainOutput.size() > 2)
            return false;

        return true;
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) final
    {
        const juce::ScopedLock sl(innerMutex);

        active = true;

        if (inner != nullptr)
        {
            inner->setRateAndBufferSizeDetails(sampleRate, samplesPerBlock);
            inner->prepareToPlay(sampleRate, samplesPerBlock);
        }
    }

    void releaseResources() final
    {
        const juce::ScopedLock sl(innerMutex);

        active = false;

        if (inner != nullptr)
            inner->releaseResources();
    }

    void reset() final
    {
        const juce::ScopedLock sl(innerMutex);

        if (inner != nullptr)
            inner->reset();
    }

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) final
    {
        jassert(!isUsingDoublePrecision());
    }

    void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) final
    {
        jassert(isUsingDoublePrecision());
    }

    bool hasEditor() const override { return true; }
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }

    const juce::String getName() const final { return "SerumInterfaceDemo"; }
    bool acceptsMidi() const final { return true; }
    bool producesMidi() const final { return true; }
    double getTailLengthSeconds() const final { return 0.0; }

    int getNumPrograms() final { return 0; }
    int getCurrentProgram() final { return 0; }
    void setCurrentProgram(int) final {}
    const juce::String getProgramName(int) final { return "None"; }
    void changeProgramName(int, const juce::String&) final {}

    void getStateInformation(juce::MemoryBlock& destData) final
    {
        const juce::ScopedLock sl(innerMutex);

        juce::XmlElement xml("state");

        if (inner != nullptr)
        {
            xml.setAttribute(editorStyleTag, (int)editorStyle);
            xml.addChildElement(inner->getPluginDescription().createXml().release());
            xml.addChildElement([this]
                {
                    juce::MemoryBlock innerState;
                    inner->getStateInformation(innerState);

                    auto stateNode = std::make_unique<juce::XmlElement>(innerStateTag);
                    stateNode->addTextElement(innerState.toBase64Encoding());
                    return stateNode.release();
                }());
        }

        const auto text = xml.toString();
        destData.replaceAll(text.toRawUTF8(), text.getNumBytesAsUTF8());
    }

    void setStateInformation(const void* data, int sizeInBytes) final
    {
        const juce::ScopedLock sl(innerMutex);

        auto xml = juce::XmlDocument::parse(juce::String(juce::CharPointer_UTF8(static_cast<const char*>(data)), (size_t)sizeInBytes));

        if (auto* pluginNode = xml->getChildByName("PLUGIN"))
        {
            juce::PluginDescription pd;
            pd.loadFromXml(*pluginNode);

            juce::MemoryBlock innerState;
            innerState.fromBase64Encoding(xml->getChildElementAllSubText(innerStateTag, {}));

            setNewPlugin(pd,
                (EditorStyle)xml->getIntAttribute(editorStyleTag, 0),
                innerState);
        }
    }

    void setNewPlugin(const juce::PluginDescription& pd, EditorStyle where, const juce::MemoryBlock& mb = {})
    {
        const juce::ScopedLock sl(innerMutex);

        const auto callback = [this, where, mb](std::unique_ptr<juce::AudioPluginInstance> instance, const juce::String& error)
            {
                if (error.isNotEmpty())
                {
                    auto options = juce::MessageBoxOptions::makeOptionsOk(juce::MessageBoxIconType::WarningIcon,
                        "Plugin Load Failed",
                        error);
                    messageBox = juce::AlertWindow::showScopedAsync(options, nullptr);
                    return;
                }

                inner = std::move(instance);
                editorStyle = where;

                if (inner != nullptr && !mb.isEmpty())
                    inner->setStateInformation(mb.getData(), (int)mb.getSize());

                if (active)
                {
                    inner->setRateAndBufferSizeDetails(getSampleRate(), getBlockSize());
                    inner->prepareToPlay(getSampleRate(), getBlockSize());
                }

                juce::NullCheckedInvocation::invoke(pluginChanged);
            };

        pluginFormatManager.createPluginInstanceAsync(pd, getSampleRate(), getBlockSize(), callback);
    }

    void clearPlugin()
    {
        const juce::ScopedLock sl(innerMutex);

        inner = nullptr;
        juce::NullCheckedInvocation::invoke(pluginChanged);
    }

    bool isPluginLoaded() const
    {
        const juce::ScopedLock sl(innerMutex);
        return inner != nullptr;
    }

    std::unique_ptr<juce::AudioProcessorEditor> createInnerEditor() const
    {
        const juce::ScopedLock sl(innerMutex);
        return rawToUniquePtr(inner->hasEditor() ? inner->createEditorIfNeeded() : nullptr);
    }

    EditorStyle getEditorStyle() const noexcept { return editorStyle; }

    //==============================================================================
    // Component overrides

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
        g.drawText("Serum Interface Component", getLocalBounds(), juce::Justification::centred);
    }

    void resized() override
    {
        // Resize child components here (if any)
    }

private:
    juce::CriticalSection innerMutex;
    std::unique_ptr<juce::AudioPluginInstance> inner;
    EditorStyle editorStyle = EditorStyle{};
    bool active = false;
    juce::ScopedMessageBox messageBox;

    juce::ApplicationProperties appProperties;
    juce::AudioPluginFormatManager pluginFormatManager;
    juce::KnownPluginList pluginList;
    std::function<void()> pluginChanged;

    static constexpr const char* innerStateTag = "inner_state";
    static constexpr const char* editorStyleTag = "editor_style";
};
