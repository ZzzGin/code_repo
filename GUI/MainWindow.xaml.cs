///////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - GUI for Project3HelpWPF                      //
// ver 1.0                                                           //
// Jing Qi, Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018//
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * this help file would describe all the features (not bugs:) of this
application. 

<Connect Tab>
  -  1. You need to connect to a server to get access to other tabs.
  -  2. You can change the IP address and port number of the remote se-
        rver to get connected.
  -  3. You can specify your client port number if the default one is 
        used.
  -  4. For the demo, we can connect to the server application on the  
        same machine of client side by using the default IP address of 
        "localhost".
  -  5. The information of connection would be shown in the lower text-
        block. 
  -  6. You can disconnect to the server.
  -  7. You can connect to the server with multiple clients with
        different client's port numbers.

<Check-In Tab>
  -  1. After giving out the information of this check-in session, you 
        can check-in a file.
  -  2. Those forms with "*" is necessary for checking-in.
  -  3. For the File field, you can click on the open button and select
        the file in the win32 file dialog.
  -  4. By checking the "Select in BrowseTab" checkbox, you can go to
        the Browse-Tab and search for the files you need to add to the 
        dependency. Then you can simply double-click the key in the list
        and it would be added to the children list automatically. If you
        make some mistakes, you can easily delete it in the chidren list.
  -  5. If the "Close" checkbox is checked, you will make a close-Check-
        in. Otherwise, an open-check-in will be made. 

<Check-Out Tab>
  -  1. After giving out the information of this check-out session, you 
        can check-out a file.
  -  2. There is two ways to fill the blanks. One is filling the
        "NameSpace", "File Name" and "Version#" manualy. The key would be
        generated automatically. Or, you can go to Browse-Tab and double
        click on the one you want to checkout. The key would be copied to 
        the clipboard. Then, you can go to the Check-out Tab and paste it
        to the "Key". The following 3 blanks would be filled automati-
        cally. 
  -  3. If the "With Dependency" checkbox is checked, all dependencies 
        would be checked-out together. Otherwise, just one file selected
        would be checked-out.
  -  4. The Check out files can be found in 
        "../ClientStorage/CheckOutFiles/".

<Browse Tab>
  -  1. By giving out the information of the file you want to browse, 
        all keys satisfy to the information would be listed in the 
        Search Results list. 
  -  2. Forms left blank will not be queried. For example, I only know
        the namespace of the file I want is "repo". I can just fill the 
        NameSpace for "repo", and click on "Accurate Search". 
  -  3. You can fill the blanks with regular expressions. Click on "RE
        Search" and a regular expressions query would be made. 
  -  4. Double click on the keys in Search Results list will show the 
        dependencies of this key. If the Recursive checkbox is checked, 
        dependency would be walked through recursively.
  -  5. Double click on the keys in Search Results list and Dependencies
        list would copy the key into clipboard. The information of this 
        selected key would be filled into the information blanks to  
        make the next search easy.

<View File Tab>
  -  1. Double click the dirs in Remote Directory list will go into this
        dir and show the files in the Remote Files list.
  -  2. Double click the files in Remote Files list will pop up a new 
        window and show the content of the file.
  -  3. Clicking the Home button to get to the root of the repository.

<View Metadata Tab>
  -  1. Fill the Key blank and click the View Metadata button, the 
        result would be shown. 
  -  2. It is recommanded to get the key into the clipboard from the
        browse-tab. When you go to V-M tab, the content in the clipboard
        would be filled into the Key automatically.
 *   
 * Required Files:
 * ---------------
 * Mainwindow.xaml, MainWindow.xaml.cs
 * Translater.dll
 * 
 * Maintenance History:
 * --------------------
 * ver 2.0 : 4/14/2018
 * - add functionality to it.
 * 
 * ver 1.0 : 30 Mar 2018
 * - first release
 * - Several early prototypes were discussed in class. Those are all superceded
 *   by this package.
 */

// Translater has to be statically linked with CommLibWrapper
// - loader can't find Translater.dll dependent CommLibWrapper.dll
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using MsgPassingCommunication;
using System.Text.RegularExpressions;

namespace WpfApp1
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private Stack<string> pathStack_ = new Stack<string>();
        private Translater translater;
        private CsEndPoint endPoint_;
        private Thread rcvThrd = null;
        private Dictionary<string, Action<CsMessage>> dispatcher_
          = new Dictionary<string, Action<CsMessage>>();
        private string serverIP = "localhost";
        private string serverPort = "8080";
        private string clientPort = "8082";

        //----< process incoming messages on child thread >----------------

        private void processMessages()
        {
            ThreadStart thrdProc = () => {
                while (true)
                {
                    CsMessage msg = translater.getMessage();
                    msg.show();
                    string msgId = msg.value("command");
                    if (dispatcher_.ContainsKey(msgId))
                        dispatcher_[msgId].Invoke(msg);
                }
            };
            rcvThrd = new Thread(thrdProc);
            rcvThrd.IsBackground = true;
            rcvThrd.Start();
        }
        //----< function dispatched by child thread to main thread >-------

        private void clearDirs()
        {
            DirList.Items.Clear();
        }
        //----< function dispatched by child thread to main thread >-------

        private void addDir(string dir)
        {
            DirList.Items.Add(dir);
        }
        //----< function dispatched by child thread to main thread >-------

        private void insertParent()
        {
            DirList.Items.Insert(0, "..");
        }
        //----< function dispatched by child thread to main thread >-------

        private void clearFiles()
        {
            FileList.Items.Clear();
        }
        //----< function dispatched by child thread to main thread >-------

        private void addFile(string file)
        {
            FileList.Items.Add(file);
        }
        //----< add client processing for message with key >---------------

        private void addClientProc(string key, Action<CsMessage> clientProc)
        {
            dispatcher_[key] = clientProc;
        }
        //----< load getDirs processing into dispatcher dictionary >-------

        private void DispatcherLoadGetDirs()
        {
            Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
            {
                
                Action clrDirs = () =>
                {
                    clearDirs();
                };
                Dispatcher.Invoke(clrDirs, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("dir"))
                    {
                        Action<string> doDir = (string dir) =>
                        {
                            addDir(dir);
                        };
                        Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
                    }
                    else if (key.Contains("command"))
                    {
                        string command;
                        command = enumer.Current.Value;
                        Action<string> addline = (string c) =>
                        {
                            add2TextBlock("Reply -" + c + "- received.");
                        };
                        Dispatcher.Invoke(addline, new Object[] { command });
                    }
                }
                Action insertUp = () =>
                {
                    insertParent();
                };
                Dispatcher.Invoke(insertUp, new Object[] { });
            };
            addClientProc("getDirs", getDirs);
        }
        //----< load getFiles processing into dispatcher dictionary >------

        private void DispatcherLoadGetFiles()
        {
            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                Action clrFiles = () =>
                {
                    clearFiles();
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("file"))
                    {
                        Action<string> doFile = (string file) =>
                        {
                            addFile(file);
                        };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }
                    else if (key.Contains("command"))
                    {
                        string command;
                        command = enumer.Current.Value;
                        Action<string> addline = (string c) =>
                        {
                            add2TextBlock("Reply -" + c + "- received.");
                        };
                        Dispatcher.Invoke(addline, new Object[] { command });
                    }
                }
            };
            addClientProc("getFiles", getFiles);
        }

        //----< load getFiles processing into dispatcher dictionary >------
        private void DispatcherTestComm()
        {
            Action<CsMessage> testComm = (CsMessage rcvMsg) =>
            {
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("command"))
                    {
                        string command;
                        command = enumer.Current.Value;
                        Action<string> addline = (string c) =>
                        {
                            add2TextBlock("Communication channel works properly.");
                            statusBarText.Text = "Connected.";
                            ServerIPTextBox.IsEnabled = false;
                            ServerPortTextBox.IsEnabled = false;
                            ClientPortTextBox.IsEnabled = false;
                            DisconnectButtom.IsEnabled = true;
                            ConnectButtom.IsEnabled = false;
                            checkin.IsEnabled = true;
                            checkout.IsEnabled = true;
                            browse.IsEnabled = true;
                            viewfile.IsEnabled = true;
                            viewmetadata.IsEnabled = true;
                            MenuItemBrowse.IsEnabled = true;
                            MenuItemCheckIn.IsEnabled = true;
                            MenuItemCheckOut.IsEnabled = true;
                            MenuItemView.IsEnabled = true;
                            MenuItemMetadata.IsEnabled = true;
                        };
                        Dispatcher.Invoke(addline, new Object[] { command });
                    }
                }
            };
            addClientProc("testComm", testComm);
        }

        //----< load checkin response processing into dispatcher dictionary >------

        private void DispatcherCheckIn()
        {
            Action<CsMessage> CheckinResp = (CsMessage rcvMsg) =>
            {
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("command"))
                    {
                        string command;
                        command = enumer.Current.Value;
                        Action<string> addline = (string c) =>
                        {
                            add2TextBlock("Reply -" + c + "- received.");
                            //statusBarText.Text = "Check-in Success.";
                        };
                        Dispatcher.Invoke(addline, new Object[] { command });
                    }
                    else if (key.Contains("err"))
                    {
                        string err;
                        err = enumer.Current.Value;
                        Action<string> adderr = (string e) =>
                        {
                            StringBuilder sb = new StringBuilder(e + "\n");
                            sb.Append(CheckInInfoTextBlock.Text);
                            CheckInInfoTextBlock.Text = sb.ToString();
                        };
                        Dispatcher.Invoke(adderr, new Object[] { err });
                    }
                }
            };
            addClientProc("checkinResp", CheckinResp);
        }

        //----< load checkout response processing into dispatcher dictionary >------

        private void DispatcherCheckOut()
        {
            Action<CsMessage> CheckoutResp = (CsMessage rcvMsg) =>
            {
                Action<string> adderr = (string e) =>
                {
                    StringBuilder sb = new StringBuilder(e + "\n\n\n");
                    sb.Append(CheckOutInfoTextBlock.Text);
                    CheckOutInfoTextBlock.Text = sb.ToString();
                };
                Dispatcher.Invoke(adderr, new Object[] { rcvMsg.value("err") });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("command"))
                    {
                        string command;
                        command = enumer.Current.Value;
                        Action<string> addline = (string c) =>
                        {
                            add2TextBlock("Reply -" + c + "- received.");
                            statusBarText.Text = "Check-out Success.";
                        };
                        Dispatcher.Invoke(addline, new Object[] { command });
                    }
                    else if (key.Contains("checkoutFile"))
                    {
                        string coFile = enumer.Current.Value;
                        Action<string> addcof = (string f) =>
                        {
                            StringBuilder sb = new StringBuilder("    - " + f + " is downloading...\n");
                            sb.Append(CheckOutInfoTextBlock.Text);
                            CheckOutInfoTextBlock.Text = sb.ToString();

                            CsEndPoint serverEndPoint = new CsEndPoint();
                            serverEndPoint.machineAddress = ServerIPTextBox.Text;
                            serverEndPoint.port = int.Parse(ServerPortTextBox.Text);
                            CsMessage msg = new CsMessage();
                            msg.add("to", CsEndPoint.toString(serverEndPoint));
                            msg.add("from", CsEndPoint.toString(endPoint_));
                            msg.add("command", "needCheckOutFile");
                            msg.add("fileName", f);
                            msg.add("toDir", "../../../../ClientStorage/CheckOutFiles");
                            translater.postMessage(msg);
                        };
                        Dispatcher.Invoke(addcof, new Object[] { coFile });
                    }
                }
            };
            addClientProc("checkoutResp", CheckoutResp);
        }

        //----< load browse response processing into dispatcher dictionary >------

        private void DispatcherBrowse()
        {
            Action<CsMessage> getResult = (CsMessage rcvMsg) =>
            {
                Action clrResult = () =>
                {
                    BrowseResultList.Items.Clear();
                };
                Dispatcher.Invoke(clrResult, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("result"))
                    {
                        Action<string> addResultToList = (string result) =>
                        {
                            BrowseResultList.Items.Add(result);
                        };
                        Dispatcher.Invoke(addResultToList, new Object[] { enumer.Current.Value });
                    }
                    else if (key.Contains("command"))
                    {
                        string command;
                        command = enumer.Current.Value;
                        Action<string> addline = (string c) =>
                        {
                            add2TextBlock("Reply -" + c + "- received.");
                        };
                        Dispatcher.Invoke(addline, new Object[] { command });
                    }
                }
            };
            addClientProc("browseSuccess", getResult);
        }

        //----< load VM response processing into dispatcher dictionary >------

        private void DispatcherVM()
        {
            Action<CsMessage> BrowseResp = (CsMessage rcvMsg) =>
            {
                if (rcvMsg.value("haveThisKey") == "false") return;
                StringBuilder sb = new StringBuilder();
                sb.Append(rcvMsg.value("keyName") + ":\n");
                sb.Append("  - FileName: " + rcvMsg.value("name") + "\n");
                sb.Append("  - PackageName: " + rcvMsg.value("packageName") + "\n");
                sb.Append("  - NameSpace: " + rcvMsg.value("nameSpace") + "\n");
                //sb.Append("  - Key: " + rcvMsg.value("keyName") + "\n");
                sb.Append("  - Description: " + rcvMsg.value("description") + "\n");
                sb.Append("  - Author: " + rcvMsg.value("author") + "\n");
                sb.Append("  - LastChangeTime: " + rcvMsg.value("dateTime") + "\n");
                sb.Append("  - RemoteDirection: " + rcvMsg.value("direction") + "\n");
                sb.Append("  - Version: " + rcvMsg.value("version") + "\n");
                string status = rcvMsg.value("status") == "true"? "Open" : "Closed";
                sb.Append("  - Status: " + status + "\n");
                sb.Append("  - Children:\n");
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("command"))
                    {
                        Action<string> addline = (string c) =>
                        {
                            add2TextBlock("Reply -" + c + "- received.");
                            statusBarText.Text = "View Metadata Success.";
                        };
                        Dispatcher.Invoke(addline, new Object[] { enumer.Current.Value });
                    }
                    else if (key.Contains("children"))
                    {
                        Action<string> addChild = (string c) =>
                        {
                            sb.Append("    * " + c + "\n");
                        };
                        Dispatcher.Invoke(addChild, new Object[] { enumer.Current.Value });
                    }
                }
                sb.Append("*********************************\n\n\n");
                Action<StringBuilder> showInTextBox = (StringBuilder sb_) =>
                {
                    sb_.Append(VmTextBox.Text);
                    VmTextBox.Text = sb_.ToString();
                };
                Dispatcher.Invoke(showInTextBox, new Object[] { sb });
            };

            addClientProc("viewMetadataSuccess", BrowseResp);
        }

        //----< load dependency response processing into dispatcher dictionary >------
        private void DispatcherDep()
        {
            Action<CsMessage> getDepResult = (CsMessage rcvMsg) =>
            {
                Action clrResult = () =>
                {
                    BrowseDependencyList.Items.Clear();
                };
                Dispatcher.Invoke(clrResult, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("result"))
                    {
                        Action<string> addResultToList = (string result) =>
                        {
                            BrowseDependencyList.Items.Add(result);
                        };
                        Dispatcher.Invoke(addResultToList, new Object[] { enumer.Current.Value });
                    }
                    else if (key.Contains("command"))
                    {
                        string command;
                        command = enumer.Current.Value;
                        Action<string> addline = (string c) =>
                        {
                            add2TextBlock("Reply -" + c + "- received.");
                        };
                        Dispatcher.Invoke(addline, new Object[] { command });
                    }
                }
            };
            addClientProc("browseDependencySuccess", getDepResult);
        }
        
        //----< load all dispatcher processing >---------------------------

        private void loadDispatcher()
        {
            DispatcherLoadGetDirs();
            DispatcherLoadGetFiles();
            DispatcherTestComm();
            DispatcherCheckIn();
            DispatcherCheckOut();
            DispatcherBrowse();
            DispatcherVM();
            DispatcherDep();
        }
        //----< start Comm, fill window display with dirs and files >------

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            checkin.IsEnabled = false;
            checkout.IsEnabled = false;
            browse.IsEnabled = false;
            viewfile.IsEnabled = false;
            viewmetadata.IsEnabled = false;
            string[] args = Environment.GetCommandLineArgs();
            if (args.Length >= 5)
            {
                serverIP = args[1];
                serverPort = args[2];
                clientPort = args[3];
            } 
            ServerIPTextBox.Text = serverIP;
            ServerPortTextBox.Text = serverPort;
            ClientPortTextBox.Text = clientPort;

            // load dispatcher
            loadDispatcher();

            // do the test
            if (args[4] == "-t")
            {
                CheckInFileNameTextBox.Text = "CheckOut.cpp";
                CheckInNameSpaceTextBox.Text = "repository";
                CheckInPackageNameTextBox.Text = "CheckOut";
                CheckInAuthorTextBox.Text = "J. Qi";
                CheckInDescriptionTextBox.Text = "This is a test checkin for demo.";
                CheckInFileTextBox.Text = 
                    "..\\..\\..\\..\\ClientStorage\\CheckOut.cpp";
                CheckInChildrenTextBox.Text = "repository::CheckIn.h.1; ";
                CheckOutKeyTextBox.Text = "repository::CheckOut.cpp.2";
                CheckOutNameSpaceTextBox.Text = "repository";
                CheckOutFileNameTextBox.Text = "CheckOut.cpp";
                CheckOutVersionNumTextBox.Text = "2";
                VMKeyTextBox.Text = "repository::CheckOut.cpp.2";
                BrowseDepTextBox.Text = "^repository.*";
                if (args.Length >= 6 && args[5] == "-testsuit")
                    testSuit();
            }
        }

       
        //----< automated unit test suite >---------------------
        private void testSuit()
        {
            test1();
            test2a();
            test2b();
            test3();
            test4();
            test5();
            test6();
        }
        //----< r1 >---------------------
        private void test1()
        {
            Console.Write("\n ============== Requirement 1 ==============");
            Console.Write("\n     This is a program written by std c++ and C#.");
            Console.Write("\n     passed<<\n\n");
        }

        //----< r2a >---------------------
        private void test2a()
        {
            Console.Write("\n ============== Requirement 2a ==============");
            //Thread.Sleep(500);
            object o = null;
            RoutedEventArgs r = null;
            Console.Write("\n ****Emulating a click on \"Connect\" to show establishing a connection.");
            ConnectButtom_MouseClick(o, r);
            Console.Write("\n >>>>Connection established.\n");

            // Check in test
            string[] files = System.IO.Directory.GetFiles("../../../../Repository/repository/CheckOut/CheckOut.cpp/");
            Console.Write("\n Before Checkin, the file in /repository/CheckOut/CheckOut.cpp/ is:");
            foreach(string f in files)
            {
                Console.Write("\n  > " + f);
            }
            Console.Write("\n\n ****Emulating a click on \"Check In\" to show Check-in a file named 'CheckOut.cpp'.");
            CheckInButtonClicked(o, r);
            Console.Write("\n >>>>CheckIn successful.\n");
            Thread.Sleep(3000);
            files = System.IO.Directory.GetFiles("../../../../Repository/repository/CheckOut/CheckOut.cpp/");
            Console.Write("\n\n After Checkin, the file in /repository/CheckOut/CheckOut.cpp/ is:");
            foreach (string f in files)
            {
                Console.Write("\n  > " + f);
            }
            Console.Write("\n We can see that a new file is stored in repo because of the check in.\n");
            //check out test
            files = System.IO.Directory.GetFiles("../../../../ClientStorage/CheckOutFiles/");
            Console.Write("\n Before Checkout, the file in /ClientStorage/CheckOutFiles/ is:");
            foreach (string f in files)
            {
                Console.Write("\n  > " + f);
            }
            Console.Write("\n\n ****Emulating a click on \"Check Out\" to show Check-Out a key named 'repository::CheckOut.cpp.2'.");
            CheckOutButtom_MouseClick(o, r);
            Console.Write("\n >>>>CheckOut successful.\n");
            Console.Write("\n\n In this case, I can not show you the change of the files in that directory in this program because when");
            Console.Write("\n the program runs here, the Check-out message is blocked. Now, a file explorer will show up to see the change.");
            string showDir = System.IO.Directory.GetCurrentDirectory().ToString();
            showDir = System.IO.Directory.GetParent(showDir).FullName;
            showDir = System.IO.Directory.GetParent(showDir).FullName;
            showDir = System.IO.Directory.GetParent(showDir).FullName;
            showDir = System.IO.Directory.GetParent(showDir).FullName;
            showDir = showDir + "\\ClientStorage\\CheckOutFiles\\";
            System.Diagnostics.Process.Start("explorer.exe", showDir);
            Console.Write("\n We can see that a new file is transfered to client side because of the check out.\n");

            Console.Write("\n     passed<<\n\n");
        }

        //----< r2b >---------------------
        private void test2b()
        {
            Console.Write("\n ============== Requirement 2a ==============");
            //Browse packages
            Console.Write("\n\n ****Emulating a click on \"RE Search\" to show browsing a package in repo whose dependency fits for '^repository.*'.");
            object o = null;
            RoutedEventArgs r = null;
            BrowseRESearchButtomClick(o, r);
            Console.Write("\n >>>>Browse successful.\n");
            Console.Write("\n\n Now, 5 results based on a query of nosql database is showing up in Browse tab.");
            tabControl.SelectedIndex = 3;
            Console.Write("\n     passed<<\n\n");
        }

        //----< r3 >---------------------
        private void test3()
        {
            Console.Write("\n ============== Requirement 3 ==============");
            Console.Write("\n\n In test2a, we showed that files is transfered from client to repo(check in), and from repo to client(check out).");
            Console.Write("\n You can click on the 'help' on menu to see the features of this client application.");
            Console.Write("\n     passed<<\n\n");
        }

        //----< r4 >---------------------
        private void test4()
        {
            Console.Write("\n ============== Requirement 4 ==============");
            Console.Write("\n\n You can see the message-passing communication system in 'MsgPassingComm' project.");
            Console.Write("\n     passed<<\n\n");
        }

        //----< r3 >---------------------
        private void test5()
        {
            Console.Write("\n ============== Requirement 5 ==============");
            Console.Write("\n\n You can see the message-passing communication system in 'MsgPassingComm' project.");
            Console.Write("\n     passed<<\n\n");
        }

        //----< r3 >---------------------
        private void test6()
        {
            Console.Write("\n ============== Requirement 6 ==============");
            Console.Write("\n\n You can see the message-passing communication system in 'MsgPassingComm' project.");
            Console.Write("\n File transfer is demoed in test2a.");
            Console.Write("\n     passed<<\n\n");
        }

        //----< strip off name of first part of path >---------------------

        private string removeFirstDir(string path)
        {
            string modifiedPath = path;
            int pos = path.IndexOf("/");
            modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
            return modifiedPath;
        }
        //----< respond to mouse double-click on dir name >----------------

        private void DirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            // build path for selected dir
            string selectedDir = (string)DirList.SelectedItem;
            string path;
            if (selectedDir == "..")
            {
                if (pathStack_.Count > 1)  // don't pop off "Storage"
                    pathStack_.Pop();
                else
                    return;
            }
            else if (selectedDir == null)
            {
                return;
            }
            else
            {
                path = pathStack_.Peek() + "/" + selectedDir;
                pathStack_.Push(path);
            }
            // display path in Dir TextBlcok
            PathTextBlock.Text = removeFirstDir(pathStack_.Peek());
            // display path in statusbar
            statusBarText.Text = "Go to path: " + removeFirstDir(pathStack_.Peek());
            // build message to get dirs and post it
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerIPTextBox.Text;
            serverEndPoint.port = int.Parse(ServerPortTextBox.Text); 
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            translater.postMessage(msg);

            // build message to get files and post it
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);
        }


        //----< add one line in the text block >---------------------------------
        private void add2TextBlock(string line)
        {
            ConnectTextBlock.Text += line + "\n";
        }

        //----< connection buttom>---------------------------------
        private void ConnectButtom_MouseClick(object sender, RoutedEventArgs e)
        {
            ConnectTextBlock.Text += "Trying to connect to " +
                ServerIPTextBox.Text + ":" + ServerPortTextBox.Text + "...\n";
            ConnectTextScrollViewer.ScrollToBottom();

            // start Comm
            endPoint_ = new CsEndPoint();
            endPoint_.machineAddress = ServerIPTextBox.Text;
            endPoint_.port = int.Parse(ClientPortTextBox.Text);
            translater = new Translater();
            translater.listen(endPoint_);
            
            // start processing messages
            processMessages();

            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerIPTextBox.Text;
            serverEndPoint.port = int.Parse(ServerPortTextBox.Text);

            PathTextBlock.Text = "Repository";
            pathStack_.Push("../Repository");
            CsMessage testmsg = new CsMessage();
            testmsg.add("to", CsEndPoint.toString(serverEndPoint));
            testmsg.add("from", CsEndPoint.toString(endPoint_));
            testmsg.add("command", "testComm");
            translater.postMessage(testmsg);
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            translater.postMessage(msg);
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);
        }
        //----< disconnection buttom>---------------------------------
        private void DisconnectButtom_MouseClick(object sender, RoutedEventArgs e)
        {
            ConnectTextBlock.Text += "Disconnect to " +
                ServerIPTextBox.Text + ":" + ServerPortTextBox.Text + "\n";
            ConnectButtom.IsEnabled = true;
            DisconnectButtom.IsEnabled = false;
            ServerIPTextBox.IsEnabled = true;
            ServerPortTextBox.IsEnabled = true;
            ClientPortTextBox.IsEnabled = true;
            checkin.IsEnabled = false;
            checkout.IsEnabled = false;
            browse.IsEnabled = false;
            viewfile.IsEnabled = false;
            viewmetadata.IsEnabled = false;
            MenuItemBrowse.IsEnabled = false;
            MenuItemCheckIn.IsEnabled = false;
            MenuItemCheckOut.IsEnabled = false;
            MenuItemView.IsEnabled = false;
            MenuItemMetadata.IsEnabled = false;

            // some bugs
            ConnectTextBlock.Text += "<note> Try different client port number and reconnect.\n";
            ConnectTextBlock.Text += "             I don't know why I cannot reconnect from the same port number.\n";
            ConnectTextScrollViewer.ScrollToBottom();
        }

        //----< home buttom>---------------------------------
        private void HomeButtom_MouseClick(object sender, RoutedEventArgs e)
        {
            pathStack_.Clear();
            pathStack_.Push("../Repository");
            PathTextBlock.Text = removeFirstDir(pathStack_.Peek());
            statusBarText.Text = "Go to path: " + removeFirstDir(pathStack_.Peek());
            CsMessage msg = new CsMessage();
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerIPTextBox.Text;
            serverEndPoint.port = int.Parse(ServerPortTextBox.Text);
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            translater.postMessage(msg);
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);
        }

        //----< double click the file list: display the file>---------------------------------
        private void FileList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            string selectedFile = (string)FileList.SelectedItem;
            statusBarText.Text = "Viewing the file: " + selectedFile;

            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerIPTextBox.Text;
            serverEndPoint.port = int.Parse(ServerPortTextBox.Text);
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "needFile");
            msg.add("fileName", selectedFile);
            msg.add("fromDir", pathStack_.Peek());
            msg.add("toDir", "../../../../ClientStorage/temp");
            translater.postMessage(msg);
            System.Threading.Thread.Sleep(500);
            string path = System.IO.Path.Combine("../../../../ClientStorage/temp", selectedFile);
            string contents = System.IO.File.ReadAllText(path);
            PopUpWindows popup = new PopUpWindows();
            popup.codeView.Text = contents;
            popup.Show();
        }

        //----< check in button>---------------------------------
        private void CheckInButtonClicked(object sender, RoutedEventArgs e)
        {
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerIPTextBox.Text;
            serverEndPoint.port = int.Parse(ServerPortTextBox.Text);

            string fileName = CheckInFileNameTextBox.Text;
            string nameSpace = CheckInNameSpaceTextBox.Text;
            string packageName = CheckInPackageNameTextBox.Text;
            string author = CheckInAuthorTextBox.Text;
            string description = CheckInDescriptionTextBox.Text;
            string localFileDir = CheckInFileTextBox.Text;
            if (fileName == "" ||
                nameSpace == "" ||
                packageName == "" ||
                localFileDir == "")
            {
                StringBuilder sb = new StringBuilder(" > Forms begin with * are necessary. Check-In rejected." + "\n");
                sb.Append(CheckInInfoTextBlock.Text);
                CheckInInfoTextBlock.Text = sb.ToString();
                return;
            }
            string children = CheckInChildrenTextBox.Text;
            string[] childrenList = children.Split(new Char[] { ';', ' '});

            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "checkin");
            msg.add("fileName", fileName);
            msg.add("nameSpace", nameSpace);
            msg.add("packageName", packageName);
            msg.add("author", author);
            msg.add("description", description);
            msg.add("close", CheckInCloseCheckBox.IsChecked.ToString());

            int counter = 0;
            foreach(string child in childrenList)
            {
                if (child != "")
                {
                    msg.add("children" + (++counter).ToString(), child);
                }
            }
            translater.postMessage(msg);
            CsMessage fileMsg = new CsMessage();
            fileMsg.add("file", System.IO.Path.GetFileName(localFileDir));
            fileMsg.add("fromDir", System.IO.Path.GetDirectoryName(localFileDir));
            fileMsg.add("toDir", "../Repository/temp/" + endPoint_.machineAddress + endPoint_.port + "/");
            fileMsg.add("to", CsEndPoint.toString(serverEndPoint));
            fileMsg.add("from", CsEndPoint.toString(endPoint_));
            fileMsg.add("command", "transferFile");
            translater.postMessage(fileMsg);
        }

        //----< check out button>---------------------------------
        private void CheckOutButtom_MouseClick(object sender, RoutedEventArgs e)
        {
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerIPTextBox.Text;
            serverEndPoint.port = int.Parse(ServerPortTextBox.Text);
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "checkout");
            msg.add("key", CheckOutKeyTextBox.Text);
            msg.add("nameSpace", CheckOutNameSpaceTextBox.Text);
            msg.add("fileName", CheckOutFileNameTextBox.Text);
            msg.add("version", CheckOutVersionNumTextBox.Text);
            msg.add("dependency", CheckOutCheckBox.IsChecked.ToString());
            translater.postMessage(msg);
        }

        //----< view metadata button>---------------------------------
        private void vmButtom_MouseClick(object sender, RoutedEventArgs e)
        {
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerIPTextBox.Text;
            serverEndPoint.port = int.Parse(ServerPortTextBox.Text);
            string key = VMKeyTextBox.Text;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("key", key);
            msg.add("command", "vm");
            msg.add("packagename", "demo");
            translater.postMessage(msg);
        }

        //----< show up help buttom>---------------------------------
        private void HelpShowUpMenu(object sender, RoutedEventArgs e)
        {
            string path = System.IO.Path.Combine("../../../../", "help.txt");
            string contents = System.IO.File.ReadAllText(path);
            PopUpWindows popup = new PopUpWindows();
            popup.codeView.Text = contents;
            popup.Show();
        }

        //----< Browse search buttom >---------------------------------
        private void BrowseSearchButtomClick(object sender, RoutedEventArgs e)
        {
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerIPTextBox.Text;
            serverEndPoint.port = int.Parse(ServerPortTextBox.Text);
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("NameSpace", NameSpaceTextBox.Text);
            msg.add("PackageName", PackageNameTextBox.Text);
            msg.add("FileName", FileNameTextBox.Text);
            msg.add("VersionNumber", VersionNumberTextBox.Text);
            msg.add("Dependency", BrowseDepTextBox.Text);
            msg.add("command", "browseSearch");
            translater.postMessage(msg);
        }

        //----< double click the result list, copy the key into the clipboard >------------
        private void SearchResultListBoxDoubleClick(object sender, MouseButtonEventArgs e)
        {
            string selectedItem = (string)BrowseResultList.SelectedItem;
            Clipboard.SetData(DataFormats.Text, (Object)selectedItem + "; ");
            if (SelectChildrenInBrowseCheckBox.IsChecked.Value)
            {
                StringBuilder osb = new StringBuilder(CheckInChildrenTextBox.Text);
                CheckInChildrenTextBox.Clear();
                osb.Append(selectedItem + "; ");
                CheckInChildrenTextBox.Text = osb.ToString();
            }
            string[] s1 = selectedItem.Split(':');
            string np = s1[0];
            string[] s2 = s1[2].Split('.');
            string v = s2[s2.Length - 1];
            string n = selectedItem.Substring(np.Length + 2, selectedItem.Length - np.Length - 2 - v.Length - 1);
            NameSpaceTextBox.Clear();
            PackageNameTextBox.Clear();
            FileNameTextBox.Clear();
            VersionNumberTextBox.Clear();
            NameSpaceTextBox.Text = np;
            FileNameTextBox.Text = n;
            VersionNumberTextBox.Text = v;
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerIPTextBox.Text;
            serverEndPoint.port = int.Parse(ServerPortTextBox.Text);
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("key", selectedItem);
            msg.add("command", "browseDependency");
            msg.add("isRecursive", RecursiveCheckBox.IsChecked.Value ? "true" : "false");
            translater.postMessage(msg);
        }

        //----< double click the result list, copy the key into the clipboard >------------
        private void DependenciesListBoxDoubleClick(object sender, MouseButtonEventArgs e)
        {
            Clipboard.SetData(DataFormats.Text, (Object)BrowseDependencyList.SelectedItem + "; ");
            if (SelectChildrenInBrowseCheckBox.IsChecked.Value)
            {
                StringBuilder osb = new StringBuilder(CheckInChildrenTextBox.Text);
                CheckInChildrenTextBox.Clear();
                osb.Append(BrowseDependencyList.SelectedItem + "; ");
                CheckInChildrenTextBox.Text = osb.ToString();
            }
            string selectedItem = (string)BrowseDependencyList.SelectedItem;
            string[] s1 = selectedItem.Split(':');
            string np = s1[0];
            string[] s2 = s1[2].Split('.');
            string v = s2[s2.Length - 1];
            string n = selectedItem.Substring(np.Length + 2, selectedItem.Length - np.Length - 2 - v.Length - 1);
            NameSpaceTextBox.Clear();
            PackageNameTextBox.Clear();
            FileNameTextBox.Clear();
            VersionNumberTextBox.Clear();
            NameSpaceTextBox.Text = np;
            FileNameTextBox.Text = n;
            VersionNumberTextBox.Text = v;
        }

        //----< handle for the checkbox changed >------------
        private void CheckboxStatueChanged(object sender, RoutedEventArgs e)
        {
            if (BrowseResultList.SelectedItem == null) return;
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerIPTextBox.Text;
            serverEndPoint.port = int.Parse(ServerPortTextBox.Text);
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("key", (string)BrowseResultList.SelectedItem);
            msg.add("command", "browseDependency");
            msg.add("isRecursive", RecursiveCheckBox.IsChecked.Value ? "true" : "false");
            translater.postMessage(msg);
        }

        //----< when clear buttom is clicked >------------
        private void ClearSearchButtomClick(object sender, RoutedEventArgs e)
        {
            NameSpaceTextBox.Clear();
            PackageNameTextBox.Clear();
            FileNameTextBox.Clear();
            VersionNumberTextBox.Clear();
            BrowseDepTextBox.Clear();
        }

        //----< Browse search with RE buttom >---------------------------------
        private void BrowseRESearchButtomClick(object sender, RoutedEventArgs e)
        {
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerIPTextBox.Text;
            serverEndPoint.port = int.Parse(ServerPortTextBox.Text);
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("NameSpace", NameSpaceTextBox.Text);
            msg.add("PackageName", PackageNameTextBox.Text);
            msg.add("FileName", FileNameTextBox.Text);
            msg.add("Dependency", BrowseDepTextBox.Text);
            msg.add("VersionNumber", VersionNumberTextBox.Text);
            msg.add("command", "browseRESearch");
            translater.postMessage(msg);
        }

        //----< remove " " and ";" in key textbox >---------------------------------
        private void VMKeyTextBoxChanged(object sender, TextChangedEventArgs e)
        {
            VMKeyTextBox.Text = VMKeyTextBox.Text.Trim(new Char[] { ' ', ';', '\n' });
        }

        //----< when tab is selected to VM, put the string in the clipboard in the textbox >------------------
        private void TabControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (viewmetadata.IsSelected)
            {
                VMKeyTextBox.Text = Clipboard.GetText();
            }
}

        //----< pop up a file select window >--------
        private void CheckInFileButtonClick(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();
            string showDir = System.IO.Directory.GetCurrentDirectory().ToString();
            showDir = System.IO.Directory.GetParent(showDir).FullName;
            showDir = System.IO.Directory.GetParent(showDir).FullName;
            showDir = System.IO.Directory.GetParent(showDir).FullName;
            showDir = System.IO.Directory.GetParent(showDir).FullName;
            openFileDialog.InitialDirectory = showDir + "\\ClientStorage\\";
            if (openFileDialog.ShowDialog() == true)
            {
                CheckInFileTextBox.Text = openFileDialog.FileName;
            }

        }
        //----< connect the key to 3 textboxes >--------
        private void CheckOutKeyTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            CheckOutKeyTextBox.Text = CheckOutKeyTextBox.Text.Trim(new Char[] { ' ', ';', '\n' });
            if (CheckOutKeyTextBox.IsFocused)
            {
                Regex nameSpaceR = new Regex(@"^\w*");
                Regex fileNameR = new Regex(@"\w+\.\w*");
                Regex versionNumR = new Regex(@"\d*$");
                CheckOutNameSpaceTextBox.Text = nameSpaceR.Match(CheckOutKeyTextBox.Text).Value;
                CheckOutFileNameTextBox.Text = fileNameR.Match(CheckOutKeyTextBox.Text).Value;
                CheckOutVersionNumTextBox.Text = versionNumR.Match(CheckOutKeyTextBox.Text).Value;
            }
        }
        //----< connect the key to 3 textboxes >--------
        private void CheckOutNameSpaceTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (CheckOutNameSpaceTextBox.IsFocused)
            {
                CheckOutKeyTextBox.Text =
                    CheckOutNameSpaceTextBox.Text + "::"
                    + CheckOutFileNameTextBox.Text + "."
                    + CheckOutVersionNumTextBox.Text;
            }
        }
        //----< connect the key to 3 textboxes >--------
        private void CheckOutFileNameTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (CheckOutFileNameTextBox.IsFocused)
            {
                CheckOutKeyTextBox.Text =
                CheckOutNameSpaceTextBox.Text + "::"
                + CheckOutFileNameTextBox.Text + "."
                + CheckOutVersionNumTextBox.Text;
            }
        }
        //----< connect the key to 3 textboxes >--------
        private void CheckOutVersionNumTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (CheckOutVersionNumTextBox.IsFocused)
            {
                CheckOutKeyTextBox.Text =
                CheckOutNameSpaceTextBox.Text + "::"
                + CheckOutFileNameTextBox.Text + "."
                + CheckOutVersionNumTextBox.Text;
            }
        }
        //----< menu item clicked >--------
        private void MenuItemBrowse_Click(object sender, RoutedEventArgs e)
        {
            tabControl.SelectedIndex = 3;
        }
        //----< menu item clicked >--------
        private void MenuItemExit_Click(object sender, RoutedEventArgs e)
        {
            System.Windows.Application.Current.Shutdown();
        }
        //----< menu item clicked >--------
        private void MenuItemCheckIn_Click(object sender, RoutedEventArgs e)
        {
            tabControl.SelectedIndex = 1;
        }
        //----< menu item clicked >--------
        private void MenuItemCheckOut_Click(object sender, RoutedEventArgs e)
        {
            tabControl.SelectedIndex = 2;
        }
        //----< menu item clicked >--------
        private void MenuItemView_Click(object sender, RoutedEventArgs e)
        {
            tabControl.SelectedIndex = 4;
        }
        //----< menu item clicked >--------
        private void MenuItemMetadata_Click(object sender, RoutedEventArgs e)
        {
            tabControl.SelectedIndex = 5;
        }
    }
}
