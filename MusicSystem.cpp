#include <iostream>
#include <string>
#include <fstream>
#include <memory>

using namespace std;

class Node
{
public:
  string song;
  shared_ptr<Node> next;
  weak_ptr<Node> prev;

  Node(const string &song) : song(song), next(nullptr) {}
};

class Playlist
{
private:
  shared_ptr<Node> head;
  shared_ptr<Node> recentlyPlayed;

  void toFile(const string &song)
  {
    ofstream file("playlist.txt", ios::out | ios::app);
    file << song << endl;
  }

  void deleteFromFile(const string &song)
  {
    ifstream infile("playlist.txt");
    ofstream temp("temp.txt");
    string line;
    bool found = false;

    while (getline(infile, line))
    {
      if (line != song)
      {
        temp << line << endl;
      }
      else
      {
        found = true;
      }
    }

    infile.close();
    temp.close();
    remove("playlist.txt");
    rename("temp.txt", "playlist.txt");

    if (found)
    {
      cout << "Song has been deleted." << endl;
    }
    else
    {
      cout << "There is no song with the name you entered." << endl;
    }
  }

public:
  Playlist() : head(nullptr), recentlyPlayed(nullptr) {}

  void addSong()
  {
    string song;
    cout << "\nEnter Song name: ";
    cin >> song;
    auto newNode = make_shared<Node>(song);
    toFile(song);

    if (!head)
    {
      head = newNode;
    }
    else
    {
      auto temp = head;
      while (temp->next)
      {
        temp = temp->next;
      }
      temp->next = newNode;
      newNode->prev = temp;
    }
  }

  void addSongFromFile(const string &song)
  {
    auto newNode = make_shared<Node>(song);
    if (!head)
    {
      head = newNode;
    }
    else
    {
      auto temp = head;
      while (temp->next)
      {
        temp = temp->next;
      }
      temp->next = newNode;
      newNode->prev = temp;
    }
  }

  void deleteSong()
  {
    if (!head)
    {
      cout << "The playlist is empty." << endl;
      return;
    }

    int choice;
    cout << "Which type of delete do you want?\n1. By Search\n2. By Position\n";
    cin >> choice;

    if (choice == 1)
    {
      string song;
      cout << "Enter song to be deleted: ";
      cin >> song;

      auto temp = head;
      while (temp)
      {
        if (temp->song == song)
        {
          deleteFromFile(song);
          if (auto prev = temp->prev.lock())
          {
            prev->next = temp->next;
          }
          else
          {
            head = temp->next;
          }
          if (temp->next)
          {
            temp->next->prev = temp->prev;
          }
          return;
        }
        temp = temp->next;
      }
      cout << "Song not found." << endl;
    }
    else if (choice == 2)
    {
      int pos;
      cout << "Enter the position of the song: ";
      cin >> pos;

      auto temp = head;
      int count = 1;
      while (temp && count < pos)
      {
        temp = temp->next;
        count++;
      }

      if (!temp)
      {
        cout << "Position is out of range." << endl;
        return;
      }

      deleteFromFile(temp->song);
      if (auto prev = temp->prev.lock())
      {
        prev->next = temp->next;
      }
      else
      {
        head = temp->next;
      }
      if (temp->next)
      {
        temp->next->prev = temp->prev;
      }
    }
  }

  void displayPlaylist()
  {
    auto temp = head;
    cout << "\nPlaylist:" << endl;
    while (temp)
    {
      cout << temp->song << endl;
      temp = temp->next;
    }
  }

  void countSongs()
  {
    int count = 0;
    auto temp = head;
    while (temp)
    {
      count++;
      temp = temp->next;
    }
    cout << "Total songs: " << count << endl;
  }

  void searchSong()
  {
    string song;
    cout << "Enter song to be searched: ";
    cin >> song;

    auto temp = head;
    while (temp)
    {
      if (temp->song == song)
      {
        cout << "Song Found" << endl;
        return;
      }
      temp = temp->next;
    }
    cout << "Song Not Found" << endl;
  }

  void playSong()
  {
    string song;
    displayPlaylist();
    cout << "Choose song you wish to play: ";
    cin >> song;

    auto temp = head;
    while (temp)
    {
      if (temp->song == song)
      {
        cout << "Now Playing: " << song << endl;
        auto newPlayed = make_shared<Node>(song);
        newPlayed->next = recentlyPlayed;
        recentlyPlayed = newPlayed;
        return;
      }
      temp = temp->next;
    }
    cout << "Song Not Found" << endl;
  }

  void displayRecentlyPlayed()
  {
    auto temp = recentlyPlayed;
    cout << "Recently Played Tracks: ";
    while (temp)
    {
      cout << temp->song << endl;
      temp = temp->next;
    }
  }

  void displayLastPlayed()
  {
    if (recentlyPlayed)
    {
      cout << "Last Played Song: " << recentlyPlayed->song << endl;
    }
    else
    {
      cout << "No last played tracks." << endl;
    }
  }

  void sortPlaylist()
  {
    if (!head || !head->next)
      return;

    auto sorted = false;
    while (!sorted)
    {
      sorted = true;
      auto temp = head;
      while (temp->next)
      {
        if (temp->song > temp->next->song)
        {
          swap(temp->song, temp->next->song);
          sorted = false;
        }
        temp = temp->next;
      }
    }
  }

  void addPlaylistFromFile()
  {
    ifstream file("playlist.txt");
    string line;
    while (getline(file, line))
    {
      addSongFromFile(line);
    }
    cout << "Playlist Added from file" << endl;
  }
};

int main()
{
  Playlist playlist;
  int choice;

  cout << "\n---- Welcome to the Playlist Manager ----\n" << endl;
  cout << "Please use '_' for spaces in song names." << endl;

  do
  {
    cout << "\n1. Add New Song\n2. Delete Song\n3. Display Playlist\n4. Total Songs\n5. Search Song\n6. Play Song\n7. Recently Played List\n8. Last Played\n9. Sort Playlist\n10. Add From File\n11. Exit\n";
    cout << "\nEnter your choice: ";
    cin >> choice;

    switch (choice)
    {
    case 1:
      playlist.addSong();
      break;
    case 2:
      playlist.deleteSong();
      break;
    case 3:
      playlist.displayPlaylist();
      break;
    case 4:
      playlist.countSongs();
      break;
    case 5:
      playlist.searchSong();
      break;
    case 6:
      playlist.playSong();
      break;
    case 7:
      playlist.displayRecentlyPlayed();
      break;
    case 8:
      playlist.displayLastPlayed();
      break;
    case 9:
      playlist.sortPlaylist();
      playlist.displayPlaylist();
      break;
    case 10:
      playlist.addPlaylistFromFile();
      break;
    case 11:
      cout << "Exiting..." << endl;
      exit(0);
    default:
      cout << "Invalid choice. Please try again." << endl;
    }
  } while (choice != 11);

  return 0;
}
