#pragma once

#include "ListPage.h"

#include "ui/model/ArpSequenceListModel.h"

class ArpSequencePage : public ListPage {
public:
    ArpSequencePage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyPress(KeyPressEvent &event) override;

private:
    void contextShow(bool doubleClick = false);
    void saveContextShow(bool doubleClick = false);
    void saveContextAction(bool doubleClick = false);
    void contextAction(int index);
    void saveContextAction(int index);
    bool contextActionEnabled(int index) const;

    void initSequence();
    void copySequence();
    void pasteSequence();
    void duplicateSequence();
    void initRoute();

    void loadSequence();
    void saveSequence();
    void saveAsSequence();
    void saveSequenceToSlot(int slot);
    void loadSequenceFromSlot(int slot);

    ArpSequenceListModel _listModel;
};
