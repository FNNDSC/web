//
//
//  Description:
//      This is the widget for Fetal pipeline options
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PIPELINEOPTIONSFETAL_H
#define PIPELINEOPTIONSFETAL_H

#include "PipelineOptions.h"

namespace Wt
{
    class WLineEdit;
};

///
//  Constants
//
const int FIRST_OPTIONAL_FETAL_STAGE = 3; // Stages 1 and 2 are required
const int NUM_OPTIONAL_FETAL_STAGES = 1;

///
/// \class PipelineOptionsFetal
/// \brief Class for options to configure fetal pipeline
///
class PipelineOptionsFetal : public PipelineOptions
{
public:
    ///
    /// Constructor
    ///
    PipelineOptionsFetal(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~PipelineOptionsFetal();

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    /// Check whether the options are valid
    ///
    virtual bool validate() const;

    ///
    /// Generate command-line options string based on user choices
    ///
    virtual std::string getCommandLineString() const;

private:


private:

    /// Buttons for pipeline stages
    WCheckBox *mStageBoxes[NUM_OPTIONAL_FETAL_STAGES];

    /// Slice selection
    WLineEdit *mSliceSelectionEdit;

    /// Margin
    WLineEdit *mMarginEdit;

    /// Head circumference
    WLineEdit *mHeadCircumferenceEdit;

};

#endif // PIPELINEOPTIONSFS_H

