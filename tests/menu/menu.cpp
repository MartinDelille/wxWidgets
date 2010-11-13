///////////////////////////////////////////////////////////////////////////////
// Name:        tests/menu/menu.cpp
// Purpose:     wxMenu unit test
// Author:      wxWidgets team
// Created:     2010-11-10
// RCS-ID:      $Id$
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/menu.h"
#include <stdarg.h>

// ----------------------------------------------------------------------------
// helper
// ----------------------------------------------------------------------------

namespace
{

enum
{
    MenuTestCase_Quit = wxID_EXIT,
    MenuTestCase_About = wxID_ABOUT,
    MenuTestCase_First = 10000
};

void PopulateMenu(wxMenu* menu, const wxString& name,  size_t& itemcount)
{
    // Start at item 1 to make it human-readable ;)
    for (int n=1; n<6; ++n, ++itemcount)
    {
        wxString label = name; label << n;
        menu->Append(MenuTestCase_First + itemcount, label, label + " help string");
    }
}

void RecursivelyCountMenuItems(const wxMenu* menu, size_t& count)
{
    CPPUNIT_ASSERT( menu );

    count += menu->GetMenuItemCount();
    for (size_t n=0; n < menu->GetMenuItemCount(); ++n)
    {
        wxMenuItem* item = menu->FindItemByPosition(n);
        if (item->IsSubMenu())
        {
            RecursivelyCountMenuItems(item->GetSubMenu(), count);
        }
    }
}

} // anon namespace


// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MenuTestCase : public CppUnit::TestCase
{
public:
    MenuTestCase() {}

    virtual void setUp() { CreateFrame(); }
    virtual void tearDown() { m_frame->Destroy(); }

private:
    CPPUNIT_TEST_SUITE( MenuTestCase );
        CPPUNIT_TEST( FindInMenubar );
        CPPUNIT_TEST( FindInMenu );
        CPPUNIT_TEST( Count );
    CPPUNIT_TEST_SUITE_END();

    void CreateFrame();

    void FindInMenubar();
    void FindInMenu();
    void Count();

    wxFrame* m_frame;

    // Holds the number of menuitems contained in all the menus
    size_t m_itemCount;

    // Store here the id of a known submenu item, to be searched for later
    int m_submenuItemId;

    // and a sub-submenu item
    int m_subsubmenuItemId;

    wxArrayString m_menuLabels;

    DECLARE_NO_COPY_CLASS(MenuTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MenuTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MenuTestCase, "MenuTestCase" );

void MenuTestCase::CreateFrame()
{
    m_frame = new wxFrame(NULL, wxID_ANY, "test frame");

    wxMenu *fileMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    wxMenu *subMenu = new wxMenu;
    wxMenu *subsubMenu = new wxMenu;

    size_t itemcount = 0;

    PopulateMenu(subsubMenu, "Subsubmenu item ", itemcount);

    // Store one of its IDs for later
    m_subsubmenuItemId = MenuTestCase_First + itemcount - 2;

    PopulateMenu(subMenu, "Submenu item ", itemcount);

    // Store one of its IDs for later
    m_submenuItemId = MenuTestCase_First + itemcount - 2;

    subMenu->AppendSubMenu(subsubMenu, "Subsubmen&u", "Test a subsubmenu");

    PopulateMenu(fileMenu, "Filemenu item ", itemcount);
    // Add a 'real' item too, for future tests
    fileMenu->Append(MenuTestCase_Quit, "E&xit\tAlt-X", "Quit this program");


    PopulateMenu(helpMenu, "Helpmenu item ", itemcount);
    helpMenu->Append(MenuTestCase_About, "&About...\tF1",
                     "(Would normally) Show about dialog");
    helpMenu->AppendSubMenu(subMenu, "Sub&menu", "Test a submenu");

    // +2 for E&xit and &About, +2 for the 2 submenus
    m_itemCount = itemcount + 4;

    // Use an arraystring here, to help with future tests
    m_menuLabels.Add("&File");
    m_menuLabels.Add("&Help");

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, m_menuLabels[0]);
    menuBar->Append(helpMenu, m_menuLabels[1]);
    m_frame->SetMenuBar(menuBar);
}

void MenuTestCase::FindInMenubar()
{
    wxMenuBar* bar = m_frame->GetMenuBar();

    // Find by name:
    CPPUNIT_ASSERT( bar->FindMenu("File") != wxNOT_FOUND );
    CPPUNIT_ASSERT( bar->FindMenu("&File") != wxNOT_FOUND );
    CPPUNIT_ASSERT( bar->FindMenu("&Fail") == wxNOT_FOUND );

    // Find by menu name plus item name:
    CPPUNIT_ASSERT( bar->FindMenuItem("File", "Exit") != wxNOT_FOUND );
    CPPUNIT_ASSERT( bar->FindMenuItem("&File", "E&xit") != wxNOT_FOUND );
    // and using the menu title
    int index = bar->FindMenu("&File");
    CPPUNIT_ASSERT( index != wxNOT_FOUND );
    wxString menutitle = bar->GetMenuLabel(index);
    CPPUNIT_ASSERT( bar->FindMenuItem(menutitle, "E&xit") != wxNOT_FOUND );

    // Find by position:
    for (size_t n=0; n < bar->GetMenuCount(); ++n)
    {
        CPPUNIT_ASSERT( bar->GetMenu(n) );
    }

    // Find by id:
    wxMenu* menu = NULL;
    wxMenuItem* item = NULL;
    item = bar->FindItem(MenuTestCase_Quit, &menu);
    CPPUNIT_ASSERT( item );
    CPPUNIT_ASSERT( menu );
    // Check that the correct menu was found
    CPPUNIT_ASSERT( menu->FindChildItem(MenuTestCase_Quit) );

    // Find submenu item:
    item = bar->FindItem(m_submenuItemId, &menu);
    CPPUNIT_ASSERT( item );
    CPPUNIT_ASSERT( menu );
    // and, for completeness, a submenu one:
    item = bar->FindItem(m_subsubmenuItemId, &menu);
    CPPUNIT_ASSERT( item );
    CPPUNIT_ASSERT( menu );
}

void MenuTestCase::FindInMenu()
{
    wxMenuBar* bar = m_frame->GetMenuBar();

    // Find by name:
    wxMenu* menuFind = bar->GetMenu(0);
    CPPUNIT_ASSERT( menuFind->FindItem("Exit") != wxNOT_FOUND );
    CPPUNIT_ASSERT( menuFind->FindItem("E&xit") != wxNOT_FOUND );
    // and for submenus
    wxMenu* menuHelp = bar->GetMenu(1);
    CPPUNIT_ASSERT( menuHelp->FindItem("Submenu") != wxNOT_FOUND );
    CPPUNIT_ASSERT( menuHelp->FindItem("Sub&menu") != wxNOT_FOUND );

    // Find by position:
    for (size_t n=0; n < menuHelp->GetMenuItemCount(); ++n)
    {
        CPPUNIT_ASSERT( menuHelp->FindItemByPosition(n) );
    }

    // Find by id:
    CPPUNIT_ASSERT( menuHelp->FindItem(MenuTestCase_About) );
    CPPUNIT_ASSERT( menuHelp->FindItem(MenuTestCase_Quit) == NULL );

    for (size_t n=0; n < menuHelp->GetMenuItemCount(); ++n)
    {
        size_t locatedAt;
        wxMenuItem* itemByPos = menuHelp->FindItemByPosition(n);
        CPPUNIT_ASSERT( itemByPos );
        wxMenuItem* itemById = menuHelp->FindChildItem(itemByPos->GetId(), &locatedAt);
        CPPUNIT_ASSERT_EQUAL( itemByPos, itemById );
        CPPUNIT_ASSERT_EQUAL( locatedAt, n );
    }

    // Find submenu item:
    for (size_t n=0; n < menuHelp->GetMenuItemCount(); ++n)
    {
        wxMenuItem* item = menuHelp->FindItemByPosition(n);
        if (item->IsSubMenu())
        {
            wxMenu* submenu;
            wxMenuItem* submenuItem = menuHelp->FindItem(m_submenuItemId, &submenu);
            CPPUNIT_ASSERT( submenuItem );
            CPPUNIT_ASSERT( item->GetSubMenu() == submenu );
        }
    }
}

void MenuTestCase::Count()
{
    wxMenuBar* bar = m_frame->GetMenuBar();
    // I suppose you could call this "counting menubars" :)
    CPPUNIT_ASSERT( bar );

    CPPUNIT_ASSERT_EQUAL( bar->GetMenuCount(), 2 );

    size_t count = 0;
    for (size_t n=0; n < bar->GetMenuCount(); ++n)
    {
        RecursivelyCountMenuItems(bar->GetMenu(n), count);
    }
    CPPUNIT_ASSERT_EQUAL( count, m_itemCount );
}
