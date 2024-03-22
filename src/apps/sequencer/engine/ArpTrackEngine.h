#pragma once

#include "TrackEngine.h"
#include "SequenceState.h"
#include "SortedQueue.h"
#include "Groove.h"
#include "RecordHistory.h"
#include "model/ArpSequence.h"
#include "StepRecorder.h"
#include "model/Arpeggiator.h"

class ArpTrackEngine : public TrackEngine {
public:
    ArpTrackEngine(Engine &engine, Model &model, Track &track, const TrackEngine *linkedTrackEngine) :
        TrackEngine(engine, model, track, linkedTrackEngine),
        _arpTrack(track.arpTrack()),
        _arpeggiator(track.arpTrack().arpeggiator())
    {
        reset();
    }

    virtual Track::TrackMode trackMode() const override { return Track::TrackMode::Arp; }

    virtual void reset() override;
    virtual void restart() override;
    virtual TickResult tick(uint32_t tick) override;
    virtual void update(float dt) override;

    virtual void changePattern() override;

    virtual void monitorMidi(uint32_t tick, const MidiMessage &message) override;
    virtual void clearMidiMonitoring() override;

    virtual const TrackLinkData *linkData() const override { return &_linkData; }

    virtual bool activity() const override { return _activity; }
    virtual bool gateOutput(int index) const override { return _gateOutput; }
    virtual float cvOutput(int index) const override { return _cvOutput; }
    virtual float sequenceProgress() const override {
        return _currentStep < 0 ? 0.f : float(_currentStep - _sequence->firstStep()) / (_sequence->lastStep() - _sequence->firstStep());
    }

    const ArpSequence &sequence() const { return *_sequence; }
    bool isActiveSequence(const ArpSequence &sequence) const { return &sequence == _sequence; }

    int currentStep() const { return _currentStep; }
    int currentRecordStep() const { return _stepRecorder.stepIndex(); }
    void setCurrentRecordStep(int value) {
        _stepRecorder.setStepIndex(value);
    }

    void setMonitorStep(int index);

    Types::PlayMode playMode() const { return _arpTrack.playMode(); }

    SequenceState sequenceState() {
        return _sequenceState;
    }

    void addNote(int note, int index);
    void removeNote(int note);


private:
    void triggerStep(uint32_t tick, uint32_t divisor, bool nextStep);
    void triggerStep(uint32_t tick, uint32_t divisor);
    void recordStep(uint32_t tick, uint32_t divisor);
    int noteFromMidiNote(uint8_t midiNote) const;

    int noteIndexFromOrder(int order);
    void advanceStep();
    void advanceOctave();

    bool fill() const {
        return (_arpTrack.fillMuted() || !TrackEngine::mute()) ? TrackEngine::fill() : false;
    }

    ArpTrack &_arpTrack;

    TrackLinkData _linkData;

    ArpSequence *_sequence;
    const ArpSequence *_fillSequence;

    uint32_t _freeRelativeTick;
    SequenceState _sequenceState;
    int _currentStep;
    bool _prevCondition;

    int _monitorStepIndex = -1;

    RecordHistory _recordHistory;
    bool _monitorOverrideActive = false;
    StepRecorder _stepRecorder;

    bool _activity;
    bool _gateOutput;
    float _cvOutput;
    float _cvOutputTarget;
    bool _slideActive;
    unsigned int _currentStageRepeat;

    const Arpeggiator &_arpeggiator;

    struct Note {
        uint8_t note;
        uint32_t order;
        uint8_t index;
    };

     static constexpr int MaxNotes = 8;

    std::array<Note, MaxNotes> _notes;
    int _stepIndex;
    int _noteIndex;
    uint32_t _noteOrder;
    int8_t _noteCount;
    int8_t _noteHoldCount;
    int8_t _octave;
    int8_t _octaveDirection;



    struct Gate {
        uint32_t tick;
        bool gate;
    };

    struct GateCompare {
        bool operator()(const Gate &a, const Gate &b) {
            return a.tick < b.tick;
        }
    };

    SortedQueue<Gate, 16, GateCompare> _gateQueue;

    struct Cv {
        uint32_t tick;
        float cv;
        bool slide;
    };

    struct CvCompare {
        bool operator()(const Cv &a, const Cv &b) {
            return a.tick < b.tick;
        }
    };

    SortedQueue<Cv, 16, CvCompare> _cvQueue;
};