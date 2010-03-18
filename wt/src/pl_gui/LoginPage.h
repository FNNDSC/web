//
//
//  Description:
//      This is a widget for logging in to the web page.  It relies on the use
//		of NIS username/passwords for authentication.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <Wt/WContainerWidget>
#include <Wt/WDialog>
#include <string>

namespace Wt
{
    class WText;
    class WLineEdit;
    class WPushButton;
}
using namespace Wt;

///
/// \class LoginPage
/// \brief Widget for logging in to the web page
///
class LoginPage : public WContainerWidget
{
public:
    ///
    /// Constructor
    ///
	LoginPage(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~LoginPage();

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    ///	Handle login button clicked [slot]
    ///
    void login();

    ///
    /// User logged in [signal]
    ///
    Wt::Signal<std::string, std::string>& userLoggedIn() { return mUserLoggedIn; }

private:

    ///
    /// MD5 password encryption using openssl
    ///
    char *md5crypt(const char *passwd, const char *magic, const char *salt);


    /// Signal for when user is logged on
    Wt::Signal<std::string, std::string> mUserLoggedIn;

    /// Line edit for username
    WLineEdit *mUserNameLineEdit;

    /// Line edit for password
    WLineEdit *mPasswordLineEdit;

    /// Failure message
    WLabel *mFailureLabel;

    /// Whether user has been logged in yet
    bool mLoggedIn;

};

#endif // LOGINPAGE_H

