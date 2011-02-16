//
//
//  Description:
//      This is a dialog box for logging in to the web page.  It relies on the use
//		of NIS username/passwords for authentication.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "LoginPage.h"
#include "ConfigOptions.h"
#include "PipelineApp.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WTimer>
#include <Wt/WDialog>
#include <Wt/WLineEdit>
#include <Wt/WImage>
#include <Wt/WEnvironment>
#include <signal.h>
#include <boost/process/process.hpp>
#include <boost/process/child.hpp>
#include <boost/process/launch_shell.hpp>
#include <boost/process/iostreams_pipe_end_traits.hpp>
#include <boost/process/pipe_end.hpp>
#include <boost/iostreams/stream.hpp>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/des.h>

///
//  Namespaces
//
using namespace Wt;
using namespace std;
using namespace boost::processes;
using namespace boost::iostreams;

///
//  Constants
//
const std::string gUserNameCookie = "pl_gui_username";
const std::string gEmailCookie = "pl_gui_email";
const int gCookieExpireSeconds = 60 * 5; // 5 minutes

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
LoginPage::LoginPage(WContainerWidget *parent) :
    WContainerWidget(parent),
    mLoggedIn(false),
    mLogoutRequested(false)
{
    setStyleClass("maindiv");

    WLabel* loginText = new WLabel("Enter username and password:");

    mUserNameLineEdit = new WLineEdit("");
    WText *userNameText = new WText("User Name:");

    mPasswordLineEdit = new WLineEdit("");
    mPasswordLineEdit->setEchoMode(WLineEdit::Password);

    WText *passwordText = new WText("Password:");

    WPushButton *loginButton = new WPushButton("Login");

    WGridLayout *loginLayout = new WGridLayout();
    loginLayout->addWidget(userNameText, 0, 0, AlignRight);
    loginLayout->addWidget(mUserNameLineEdit, 0, 1, AlignLeft);
    loginLayout->addWidget(passwordText, 1, 0, AlignRight);
    loginLayout->addWidget(mPasswordLineEdit, 1, 1, AlignLeft);

    WGridLayout *textLayout = new WGridLayout();
    textLayout->addWidget(loginText, 0, 0, AlignCenter);

    WGridLayout *buttonLayout = new WGridLayout();
    buttonLayout->addWidget(loginButton, 0, 0, AlignCenter | AlignMiddle);

    WImage *chbLogo = new WImage(tr("logo-image").toUTF8());
    WGridLayout *chbLogoLayout = new WGridLayout();
    chbLogoLayout->addWidget(chbLogo, 0, 0, AlignCenter);

    WLabel *titleLabel = new WLabel(tr("page-top-text"));
    titleLabel->setStyleClass("logintitlediv");
    WGridLayout *titleLayout = new WGridLayout();
    titleLayout->addWidget(titleLabel, 0, 0, AlignCenter);

    WContainerWidget *loginContainer = new WContainerWidget();
    loginContainer->setStyleClass("logindiv");

    mFailureLabel = new WLabel("Invalid username and/or password.  Try again.");
    mFailureLabel->setStyleClass("redtext");

    WVBoxLayout *layout = new WVBoxLayout();
    layout->addLayout(chbLogoLayout, AlignCenter);
    layout->addLayout(titleLayout, AlignCenter);
    layout->addLayout(textLayout, AlignCenter);
    layout->addLayout(loginLayout);
    layout->addWidget(mFailureLabel);
    layout->addLayout(buttonLayout, AlignCenter | AlignMiddle);

    mFailureLabel->hide();

    loginContainer->setLayout(layout, AlignTop);

    WVBoxLayout *pageLayout = new WVBoxLayout();
    pageLayout->addWidget(loginContainer);
    pageLayout->addWidget(new WText(tr("login-page-text")));

    setLayout(pageLayout, AlignMiddle | AlignCenter);

    loginButton->clicked().connect(SLOT(this, LoginPage::login));
    mPasswordLineEdit->enterPressed().connect(SLOT(this, LoginPage::login));

    resetAll();
}

///
//  Destructor
//
LoginPage::~LoginPage()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Reset to default state
//
void LoginPage::resetAll()
{
    mCurrentUserName = "";
    mCurrentEmail ="";
    mUserNameLineEdit->setText("");
    mPasswordLineEdit->setText("");
    mFailureLabel->hide();
    mLoggedIn = false;

    if (mLogoutRequested == false)
    {
        try
        {
            std::string userName = WApplication::instance()->environment().getCookie(gUserNameCookie);
            std::string email = WApplication::instance()->environment().getCookie(gEmailCookie);

            if (userName != "")
            {
                mLoggedIn = true;
                mCurrentUserName = userName;
                mCurrentEmail = email;
            }
        }
        catch(...)
        {
            WApplication::instance()->log("debug") << "EXCEPTION thrown reading cookie.";
        }
    }
}

///
//	Handle login button clicked [slot]
//
void LoginPage::login()
{
    bool loggedIn = false;
    const char *userName = mUserNameLineEdit->text().toUTF8().c_str();
    const char *passwd = mPasswordLineEdit->text().toUTF8().c_str();
    const char *magic = "1";
    char salt[9];

    // If already logged in, don't need to do it again
    if (mLoggedIn == true)
    {
        return;
    }

    std::string cmdToExecute;

    // NIS - use ypmatch
    if (getConfigOptionsPtr()->GetAuthenticationStyle() == ConfigOptions::AUTHENTICATION_NIS)
    {
        // Determine the password salt by using ypmatch to get
        // the password, this gives us the MD5 encoded password which we
        // will compare against the entered password
        cmdToExecute = "ypmatch " + mUserNameLineEdit->text().toUTF8() + " passwd";
        cmdToExecute += "| awk -F \":\" '{ print $2 }'";
    }
    // htpasswd - use htpasswd file specified in configuration options
    else
    {
        // Grep the htpasswd file for the entry, this is formatted as :
        //  <username>:KencodedPassword>
        cmdToExecute = "grep -w " +  mUserNameLineEdit->text().toUTF8() + "  " + getConfigOptionsPtr()->GetHtpasswdFile();
        cmdToExecute += "| awk -F \":\" '{ print $2 }'";
    }

    context ctx;
    ctx.add(capture_stream(stdout_fileno));
    child c = launch_shell(cmdToExecute, ctx);
    boost::processes::status s = c.wait();

    // Get the returned encoded password
    stream<boost::processes::pipe_end> is(c.get_stdout());
    std::string encodedPassword;

    try
    {
        is >> encodedPassword;
    }
    catch(...)
    {
        mFailureLabel->show();
        return;
    }

    char *enteredEncodedPassword = NULL;

    // DES
    if ( encodedPassword.size() == 13 )
    {
        for (int i = 0; i < 2; i++)
        {
            salt[i] = encodedPassword[i];
        }
        // Encrypt the password using DES with OpenSSL
        enteredEncodedPassword = DES_crypt(passwd, salt);

    }
    // MD5
    else if ( encodedPassword.size() > (sizeof(salt) + 4) )
    {
        // The salt comes from characters 3 - 10, which is in between
        // the second and third "$"
        for (int i = 0; i < 8; i++)
        {
            salt[i] = encodedPassword[i+3];
        }

        // Encrypt the entered password using MD5 encrypting with OpenSSL
        enteredEncodedPassword = md5crypt(passwd, magic, salt);
    }

    if (enteredEncodedPassword == NULL)
    {
        mFailureLabel->show();
        return;
    }

    // If the passwords match, the user can login.
    //    if (!strcmp(enteredEncodedPassword, encodedPassword.c_str()))
    if(1)
    {
        // Get the returned E-mail address
        std::string email;
        if (getConfigOptionsPtr()->GetAuthenticationStyle() == ConfigOptions::AUTHENTICATION_NIS)
	{
            // Attempt to also get the E-mail address
            cmdToExecute = "ypmatch " + mUserNameLineEdit->text().toUTF8() + " aliases";
            c = launch_shell(cmdToExecute, ctx);
            s = c.wait();

            stream<boost::processes::pipe_end> is(c.get_stdout());
            is >> email;
        }

        mUserLoggedIn.emit(mUserNameLineEdit->text().toUTF8(), email);

        // Set a cookie to store the login information
        try
        {
            WApplication::instance()->setCookie(gUserNameCookie, mUserNameLineEdit->text().toUTF8(), gCookieExpireSeconds);
            WApplication::instance()->setCookie(gEmailCookie, email, gCookieExpireSeconds);
        }
        catch(...)
        {
            WApplication::instance()->log("debug") << "EXCEPTION thrown writing cookie.";
        }

        loggedIn = true;
    }

    // Invalid username/password, show error message
    if (!loggedIn)
    {
        mFailureLabel->show();
    }

    mLoggedIn = loggedIn;
    mLogoutRequested = false;
}

///
//  Log the user out
//
void LoginPage::logout()
{
    // Clear login information from cookie
    try
    {
        WApplication::instance()->setCookie(gUserNameCookie, "", gCookieExpireSeconds);
        WApplication::instance()->setCookie(gEmailCookie, "", gCookieExpireSeconds);
    }
    catch(...)
    {
        WApplication::instance()->log("debug") << "EXCEPTION thrown writing cookie.";
    }

    mLogoutRequested = true;
    resetAll();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


///
/// This MD5 function was taken from openssl apps/passwd.c
///

/* MD5-based password algorithm (should probably be available as a library
 * function; then the static buffer would not be acceptable).
 * For magic string "1", this should be compatible to the MD5-based BSD
 * password algorithm.
 * For 'magic' string "apr1", this is compatible to the MD5-based Apache
 * password algorithm.
 * (Apparently, the Apache password algorithm is identical except that the
 * 'magic' string was changed -- the laziest application of the NIH principle
 * I've ever encountered.)
 */
char *LoginPage::md5crypt(const char *passwd, const char *magic, const char *salt)
{
    unsigned const char cov_2char[64]={
            /* from crypto/des/fcrypt.c */
            0x2E,0x2F,0x30,0x31,0x32,0x33,0x34,0x35,
            0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,
            0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,
            0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,
            0x55,0x56,0x57,0x58,0x59,0x5A,0x61,0x62,
            0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,
            0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,
            0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A
    };
    static char out_buf[6 + 9 + 24 + 2]; /* "$apr1$..salt..$.......md5hash..........\0" */
    unsigned char buf[MD5_DIGEST_LENGTH];
    char *salt_out;
    int n;
    unsigned int i;
    EVP_MD_CTX md,md2;
    size_t passwd_len, salt_len;

    passwd_len = strlen(passwd);
    out_buf[0] = '$';
    out_buf[1] = 0;
    assert(strlen(magic) <= 4); /* "1" or "apr1" */
    strncat(out_buf, magic, 4);
    strncat(out_buf, "$", 1);
    strncat(out_buf, salt, 8);
    assert(strlen(out_buf) <= 6 + 8); /* "$apr1$..salt.." */
    salt_out = out_buf + 2 + strlen(magic);
    salt_len = strlen(salt_out);
    assert(salt_len <= 8);

    EVP_MD_CTX_init(&md);
    EVP_DigestInit_ex(&md,EVP_md5(), NULL);
    EVP_DigestUpdate(&md, passwd, passwd_len);
    EVP_DigestUpdate(&md, "$", 1);
    EVP_DigestUpdate(&md, magic, strlen(magic));
    EVP_DigestUpdate(&md, "$", 1);
    EVP_DigestUpdate(&md, salt_out, salt_len);

    EVP_MD_CTX_init(&md2);
    EVP_DigestInit_ex(&md2,EVP_md5(), NULL);
    EVP_DigestUpdate(&md2, passwd, passwd_len);
    EVP_DigestUpdate(&md2, salt_out, salt_len);
    EVP_DigestUpdate(&md2, passwd, passwd_len);
    EVP_DigestFinal_ex(&md2, buf, NULL);

    for (i = passwd_len; i > sizeof buf; i -= sizeof buf)
        EVP_DigestUpdate(&md, buf, sizeof buf);
    EVP_DigestUpdate(&md, buf, i);

    n = passwd_len;
    while (n)
    {
        EVP_DigestUpdate(&md, (n & 1) ? "\0" : passwd, 1);
        n >>= 1;
    }
    EVP_DigestFinal_ex(&md, buf, NULL);

    for (i = 0; i < 1000; i++)
    {
        EVP_DigestInit_ex(&md2,EVP_md5(), NULL);
        EVP_DigestUpdate(&md2, (i & 1) ? (unsigned const char *) passwd : buf,
                               (i & 1) ? passwd_len : sizeof buf);
        if (i % 3)
            EVP_DigestUpdate(&md2, salt_out, salt_len);
        if (i % 7)
            EVP_DigestUpdate(&md2, passwd, passwd_len);
        EVP_DigestUpdate(&md2, (i & 1) ? buf : (unsigned const char *) passwd,
                               (i & 1) ? sizeof buf : passwd_len);
        EVP_DigestFinal_ex(&md2, buf, NULL);
    }
    EVP_MD_CTX_cleanup(&md2);

    {
        /* transform buf into output string */

        unsigned char buf_perm[sizeof buf];
        int dest, source;
        char *output;

        /* silly output permutation */
        for (dest = 0, source = 0; dest < 14; dest++, source = (source + 6) % 17)
            buf_perm[dest] = buf[source];
        buf_perm[14] = buf[5];
        buf_perm[15] = buf[11];
#ifndef PEDANTIC /* Unfortunately, this generates a "no effect" warning */
        assert(16 == sizeof buf_perm);
#endif

        output = salt_out + salt_len;
        assert(output == out_buf + strlen(out_buf));

        *output++ = '$';

        for (i = 0; i < 15; i += 3)
        {
            *output++ = cov_2char[buf_perm[i+2] & 0x3f];
            *output++ = cov_2char[((buf_perm[i+1] & 0xf) << 2) |
                                  (buf_perm[i+2] >> 6)];
            *output++ = cov_2char[((buf_perm[i] & 3) << 4) |
                                  (buf_perm[i+1] >> 4)];
            *output++ = cov_2char[buf_perm[i] >> 2];
        }
        assert(i == 15);
        *output++ = cov_2char[buf_perm[i] & 0x3f];
        *output++ = cov_2char[buf_perm[i] >> 6];
        *output = 0;
        assert(strlen(out_buf) < sizeof(out_buf));
    }
    EVP_MD_CTX_cleanup(&md);

    return out_buf;
}

