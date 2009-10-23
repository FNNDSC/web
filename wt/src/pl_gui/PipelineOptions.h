//
//
//  Description:
//      This is the base widget for pipeline options (tract, fs, etc.).  It contains
//      the options which are common amongst all pipelines.  The other options are
//      in sub-classed objects.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PIPELINEOPTIONS_H
#define PIPELINEOPTIONS_H

#include <Wt/WContainerWidget>
#include <string>
#include "ScanBrowser.h"
#include "GlobalEnums.h"

namespace Wt
{
    class WLabel;
    class WSelectionBox;
    class WGroupBox;
    class WGridLayout;
    class WVBoxLayout;
    class WLineEdit;
}

using namespace Wt;

///
/// \class PipelineOptions
/// \brief Base class for options to configure pipeline
///
class PipelineOptions : public WContainerWidget
{
public:
    ///
    /// Constructor
    ///
    PipelineOptions(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~PipelineOptions();

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    ///  Generate command-line options string based on user choices
    ///
    virtual std::string getCommandLineString() const;

    ///
    ///  Get the current directory suffix
    ///
    virtual std::string getOutputDirSuffix() const;

    ///
    ///  Get the current file suffix
    ///
    virtual std::string getOutputFileSuffix() const;

private:


protected:

    /// Group box containing options for pipeline configuration
    WGroupBox *mPipelineOptionsBox;

    /// Layout for pipeline options box
    WGridLayout *mPipelineOptionsBoxLayout;

    /// Stage button group
    WGroupBox *mStageButtonGroup;

    /// Stage button group layout
    WVBoxLayout *mStageButtonGroupLayout;

    /// Directory group box
    WGroupBox *mDirectoryGroupBox;

    /// Directory group box layout
    WGridLayout *mDirectoryGroupBoxLayout;

    /// Output directory suffix
    WLineEdit *mOutputDirSuffix;

    /// Output file suffix
    WLineEdit *mOutputFileSuffix;

    /// E-mail user
    WLineEdit *mEmailUser;
};

#endif // PIPELINEOPTIONS_H

