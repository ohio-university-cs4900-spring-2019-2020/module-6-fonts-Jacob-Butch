#pragma once

#include "GUI.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {

    class ProgressBar {
    public:
        static ProgressBar* New();
        virtual ~ProgressBar() {};

        float getProgressWidth() { return progress->getWidthGUI(); };
        void setProgressWidth(float width) { progress->setWidthGUI(width); };
        void addToProgress(float toAdd) { progress->setWidthGUI(getProgressWidth() + toAdd); };

        GUI* bar;
        GUI* progress;

        bool fill = false;
        bool empty = false;

        void fillBar();
        void emptyBar();
        void toggleFill();
        void toggleEmpty();
        void reset();
        bool isFull();

    protected:
        ProgressBar();

    };
}

#endif