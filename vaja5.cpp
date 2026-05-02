#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

bool DEBUG = false;

class BinWriter
{
public:
    int k;
    ofstream f;
    unsigned char x;

    BinWriter(const char *p) : k(0), x(0)
    {
        f.open(p, ios::binary);
    }

    ~BinWriter()
    {
        if (k > 0)
        {
            f.write((char *)&x, 1);
        }
        if (f.is_open())
            f.close();
    }

    void writeByte(unsigned char val)
    {
        f.write((char *)&val, 1);
    }

    void writeInt(int y)
    {
        f.write((char *)&y, 4);
    }

    void writeFloat(float val)
    {
        f.write((char *)&val, 4);
    }

    void writeBit(bool b)
    {
        if (b)
        {
            x ^= (-b ^ x) & (1 << k);
        }

        k++;

        if (k == 8)
        {
            writeByte(x);
            k = 0;
            x = 0;
        }
    }
};

class BinReader
{
public:
    int k;
    ifstream f;
    unsigned char x;

    BinReader(const char *p) : k(8), x(0)
    {
        f.open(p, ios::binary);
    }

    bool readBit()
    {
        if (k == 8)
        {
            f.read((char *)&x, 1);
            k = 0;
        }

        bool b = (x >> k) & 1;
        k++;

        return b;
    }

    unsigned char readByte()
    {
        f.read((char *)&x, 1);
        return x;
    }

    int readInt()
    {
        int y;
        f.read((char *)&y, 4);
        return y;
    }

    float readFloat()
    {
        float val;
        f.read((char *)&val, 4);
        return val;
    }
};

struct Znak
{
    unsigned char znak;
    int frekvenca; // Kolikokrat se pojavi
    string koda;   // Shannon-Fano koda
};

vector<unsigned char> preberiDatoteko(const char *pot)
{
    ifstream in(pot, ios::binary);

    vector<unsigned char> podatki;
    unsigned char c;

    while (in.read((char *)&c, 1))
    {
        podatki.push_back(c);
        // cout << " ('" << c << "')";
    }

    return podatki;
}

vector<Znak> zgradiSeznam(int frekvence[256])
{
    vector<Znak> seznam;

    // Napolnimo seznam
    for (int i = 0; i < 256; i++)
    {
        if (frekvence[i] > 0)
        {
            Znak z;
            z.znak = (unsigned char)i;
            z.frekvenca = frekvence[i];
            z.koda = "";
            seznam.push_back(z);
        }
    }

    // Ročno sortiranje seznama po velikosti od največjega do najmanjšega (selection sort)
    for (size_t i = 0; i < seznam.size(); i++)
    {
        size_t maxIndex = i;

        for (size_t j = i + 1; j < seznam.size(); j++)
        {
            if (seznam[j].frekvenca > seznam[maxIndex].frekvenca)
            {
                maxIndex = j;
            }
        }

        // swap
        Znak temp = seznam[i];
        seznam[i] = seznam[maxIndex];
        seznam[maxIndex] = temp;
    }

    return seznam;
}

void izpisiFrekvence(int frekvence[256])
{
    cout << "\n--- FREKVENCNA TABELA ---" << endl;

    for (int i = 0; i < 256; i++)
    {
        if (frekvence[i] > 0)
        {
            cout << "ASCII " << i;

            if (i >= 32 && i <= 126)
                cout << " ('" << (char)i << "')";

            cout << " -> " << frekvence[i] << endl;
        }
    }
}

void izpisiKode(vector<Znak> &seznam)
{
    cout << "\n--- SHANNON-FANO KODE ---" << endl;

    for (Znak z : seznam)
    {
        cout << "ASCII " << (int)z.znak;

        if (z.znak >= 32 && z.znak <= 126)
            cout << " ('" << z.znak << "')";

        cout << " | frekvenca: " << z.frekvenca
             << " | koda: " << z.koda << endl;
    }
}
void shannonFano(vector<Znak> &seznam, int levo, int desno)
{
    // Če ima podseznam samo en znak, ga ne delimo več
    if (levo >= desno)
    {
        return;
    }

    // Izračunamo skupno frekvenco trenutnega podseznama
    int skupnaVsota = 0;
    for (int i = levo; i <= desno; i++)
    {
        skupnaVsota += seznam[i].frekvenca;
    }

    int najboljsaMeja = levo;
    int najmanjsaRazlika = skupnaVsota;
    int levaVsota = 0;

    // Iščemo mesto, kjer sta leva in desna vsota čim bolj podobni
    for (int i = levo; i < desno; i++)
    {
        levaVsota += seznam[i].frekvenca;

        int desnaVsota = skupnaVsota - levaVsota;
        int razlika = abs(levaVsota - desnaVsota);

        if (razlika <= najmanjsaRazlika)
        {
            najmanjsaRazlika = razlika;
            najboljsaMeja = i;
        }
    }

    if (DEBUG)
    {
        cout << "\nDelim seznam od indeksa " << levo << " do " << desno << endl;
        cout << "Skupna vsota: " << skupnaVsota << endl;
        cout << "Najboljsa meja na indexu: " << najboljsaMeja << endl;

        cout << "Zgornji del dobi 0: ";
        for (int i = levo; i <= najboljsaMeja; i++)
            cout << seznam[i].znak << "(" << seznam[i].frekvenca << ") ";
        cout << endl;

        cout << "Spodnji del dobi 1: ";
        for (int i = najboljsaMeja + 1; i <= desno; i++)
            cout << seznam[i].znak << "(" << seznam[i].frekvenca << ") ";
        cout << endl;
    }

    // Zgornja polovica dobi bit 0
    for (int i = levo; i <= najboljsaMeja; i++)
    {
        seznam[i].koda += "0";
    }

    // Spodnja polovica dobi bit 1
    for (int i = najboljsaMeja + 1; i <= desno; i++)
    {
        seznam[i].koda += "1";
    }

    // Rekurzivno ponovimo za oba dela
    shannonFano(seznam, levo, najboljsaMeja);
    shannonFano(seznam, najboljsaMeja + 1, desno);
}

void kompresiraj(const char *vhodnaDatoteka)
{
    // Preberemo vse bajte iz vhodne datoteke
    vector<unsigned char> podatki = preberiDatoteko(vhodnaDatoteka);

    // Inicializiramo frekvenčno tabelo (256 možnih znakov)
    int frekvence[256] = {0};

    // Preštejemo pojavitve vsakega znaka
    for (unsigned char c : podatki)
    {
        frekvence[c]++;
    }

    if (DEBUG)
    {
        izpisiFrekvence(frekvence);
    }

    // Iz frekvenc zgradimo seznam znakov
    vector<Znak> seznam = zgradiSeznam(frekvence);

    // Generiramo Shannon-Fano kode
    if (seznam.size() == 1)
    {
        seznam[0].koda = "0";
    }
    else if (seznam.size() > 1)
    {
        shannonFano(seznam, 0, seznam.size() - 1);
    }

    if (DEBUG)
    {
        izpisiKode(seznam);
    }

    // Naredimo mapo znak -> koda
    map<unsigned char, string> kodirnaTabela;

    for (Znak z : seznam)
    {
        kodirnaTabela[z.znak] = z.koda;
    }

    {
        BinWriter bw("out.bin");

        // Najprej zapišemo frekvenčno tabelo (256 * 4 byte)
        for (int i = 0; i < 256; i++)
        {
            bw.writeInt(frekvence[i]);
        }

        if (DEBUG)
        {
            cout << "\n--- KODIRANJE PODATKOV ---" << endl;
        }

        // Nato zapišemo podatke kot bite
        for (unsigned char c : podatki)
        {
            string koda = kodirnaTabela[c];

            if (DEBUG)
            {
                if (c >= 32 && c <= 126)
                    cout << c << " -> " << koda << endl;
                else
                    cout << "ASCII " << (int)c << " -> " << koda << endl;
            }

            for (char bit : koda)
            {
                bw.writeBit(bit == '1');
            }
        }
    }

    // Izračun kompresijskega razmerja
    ifstream original(vhodnaDatoteka, ios::binary | ios::ate);
    ifstream compressed("out.bin", ios::binary | ios::ate);

    int m = 0;

    for (unsigned char c : podatki)
    {
        m += kodirnaTabela[c].length();
    }

    int n = podatki.size() * 8;

    cout << "Izhodna datoteka: out.bin" << endl;

    cout << "Originalna velikost: " << n << " bitov" << endl;
    cout << "Kompresirana velikost brez frekvencne tabele: " << m << " bitov" << endl;

    if (n > 0)
    {
        cout << "Kompresijsko razmerje: " << (double)m / n << endl;
    }
}

void dekompresiraj(const char *vhodnaDatoteka)
{
    BinReader br(vhodnaDatoteka);

    int frekvence[256];
    int steviloZnakov = 0;

    // Preberemo frekvenčno tabelo
    for (int i = 0; i < 256; i++)
    {
        frekvence[i] = br.readInt();
        steviloZnakov += frekvence[i];
    }

    if (DEBUG)
    {
        izpisiFrekvence(frekvence);
    }

    // Zgradimo seznam in ponovno generiramo kode
    vector<Znak> seznam = zgradiSeznam(frekvence);

    if (seznam.size() == 1)
    {
        seznam[0].koda = "0";
    }
    else if (seznam.size() > 1)
    {
        shannonFano(seznam, 0, seznam.size() - 1);
    }

    if (DEBUG)
    {
        izpisiKode(seznam);
    }

    // Naredimo mapo koda -> znak
    map<string, unsigned char> dekodirnaTabela;

    for (Znak z : seznam)
    {
        dekodirnaTabela[z.koda] = z.znak;
    }

    ofstream out("out_d.bin", ios::binary);

    string trenutnaKoda = "";
    int zapisaniZnaki = 0;

    if (DEBUG)
    {
        cout << "\n--- DEKODIRANJE BITOV ---" << endl;
        cout << "Stevilo znakov za obnovitev: " << steviloZnakov << endl;
    }

    // Beremo bite in sestavljamo kode
    while (zapisaniZnaki < steviloZnakov)
    {
        bool bit = br.readBit();

        if (bit)
            trenutnaKoda += "1";
        else
            trenutnaKoda += "0";

        // Ko najdemo kodo, jo pretvorimo nazaj v znak
        if (dekodirnaTabela.count(trenutnaKoda))
        {
            unsigned char znak = dekodirnaTabela[trenutnaKoda];

            if (DEBUG)
            {
                cout << trenutnaKoda << " -> ";
                if (znak >= 32 && znak <= 126)
                    cout << znak;
                else
                    cout << "ASCII " << (int)znak;
                cout << endl;
            }

            out.write((char *)&znak, 1);

            trenutnaKoda = "";
            zapisaniZnaki++;
        }
    }

    out.close();

    cout << "Izhodna datoteka: out_d.bin" << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Uporaba:" << endl;
        cout << "./vaja5 c <binarna_datoteka>" << endl;
        cout << "./vaja5 d <binarna_datoteka>" << endl;
        return 1;
    }

    char izbira = argv[1][0];
    const char *pot = argv[2];

    if (izbira == 'c')
    {
        kompresiraj(pot);
    }
    else if (izbira == 'd')
    {
        dekompresiraj(pot);
    }
    else
    {
        cout << "Napaka: uporabi c za kompresijo ali d za dekompresijo." << endl;
        return 1;
    }

    return 0;
}

// Hello