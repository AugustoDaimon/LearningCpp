#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

// Joga contra si mesmo varias vezes até encontrar o jogo ideal
// Ao jogar contra si mesmo evita repetir os mesmos movimentos que ja fez

class Node
{
private:
    char board[3][3];
    int nodeMoveCounter = 0;

public:
    void jogarTurnoPlayer(int *playerMoveCounter)
    {
        // TODO: Player não deve ser capaz de sobreescrever jogada do bot
        int playerMoveColumn, playerMoveRow;

        (*playerMoveCounter)++;

        cout << endl
             << nodeMoveCounter << " - " << *playerMoveCounter << "º Turno do Jogador: (row, column; 1 a 3): ";
        cin >> playerMoveRow >> playerMoveColumn; // Human Reading (1, 2 ou 3)

        board[playerMoveRow - 1][playerMoveColumn - 1] = 'O';
    }

    void jogarTurnoComputador(char peca)
    {
        int botMoveColumn, botMoveRow;

        do
        {
            botMoveColumn = rand() % 3;
            botMoveRow = rand() % 3;
        } while (board[botMoveRow][botMoveColumn] != '-');

        // cout << "BOT: Joguei em row: " << botMoveRow << " e column: " << botMoveColumn << endl;
        board[botMoveRow][botMoveColumn] = peca;
    }

    void copiarBoard(Node nodeAtual, int nodeLevel) {
        for (int r = 0; r < 3; r++)
        {
            for (int c = 0; c < 3; c++)
            {
                board[r][c] = nodeAtual.board[r][c];
            }
        }

        nodeMoveCounter = nodeLevel;
    }

    bool comparaBoards(Node nodeComparado)
    {
        if (board == nodeComparado.board)
            return true;
        else
            return false;
    }

    bool checaCampos(char campo1, char campo2, char campo3)
    {
        if (campo1 == campo2 && campo2 == campo3)
        {
            if (campo1 == '-')
                return false;
            else
            {
                // cout << endl  << campo1 << " " << campo2 << "" " << campo3 << endl;
                return true;
            }
        }
        else
            return false;
    }

    int verificarStatus()
    {
        // 0 - Normal, 1 - Vitoria Player, 2 - Vitoria Bot, 3 - Empate

        if (checaCampos(board[0][0], board[1][1], board[2][2])    // Diagonal Topo Esquerdo
            || checaCampos(board[2][0], board[1][1], board[0][2]) // Diagonal Topo Direito
            || checaCampos(board[0][1], board[1][1], board[2][1]) // Central Vertical
            || checaCampos(board[1][0], board[1][1], board[1][2]) // Central Horizontal
            || checaCampos(board[0][0], board[1][0], board[2][0]) // Vertical Esquerda
            || checaCampos(board[0][2], board[1][2], board[2][2]) // Vertical Direita
            || checaCampos(board[0][0], board[0][1], board[0][2]) // Horizontal Topo
            || checaCampos(board[2][0], board[2][1], board[2][2]) // Central Horizontal
        )
        {
            cout << endl
                 << "FIM DO JOGO, VENCEDOR: ";
            if (nodeMoveCounter % 2 == 1)
            {
                cout << "PLAYER";
                return 1;
            }
            else
            {
                cout << "BOT";
                return 2;
            }
        }
        else
        {
            if (nodeMoveCounter == 9)
            {
                cout << endl
                     << "FIM DO JOGO, EMPATE!";
                return 3;
            }
            else
            {
                return 0;
            }
        }
    }

    void limparBoard()
    {
        for (int r = 0; r < 3; r++)
        {
            for (int c = 0; c < 3; c++)
            {
                board[r][c] = '-';
            }
        }
    }

    void imprimirBoard()
    {
        for (int r = 0; r < 3; r++)
        {
            for (int c = 0; c < 3; c++)
            {
                cout << board[r][c] << " ";
            }
            cout << endl;
        }
    }

    int getMoveCounter()
    {
        return nodeMoveCounter;
    }

    void addToMoveCounter()
    {
        nodeMoveCounter++;
    }
};

// ply = game computing jargon for one turn by one player
// vale a pena olhar: https://courses.grainger.illinois.edu/cs440/fa2019/Lectures/lect33.html
class Ply
{
public:
    Node nodePly; // Jogada Atual
    vector<Ply*> plyChilds; // Possiveis Derivações
    Node nodeAux;
    int nodeLevelCounter;
    int weight;

    Ply(){}

    Ply(Node node, int level)
    {
        nodePly = node;
        nodeLevelCounter = level;
    }

    // Faz uma *JOGADA* criando um novo Board, se o board for o msm de algum child vai para ele
    // Se o board não existe, cria um novo child com o novo board no nivel abaixo
    int novoJogo()
    {
        //cout << "NOVA JOGADA\n";
        if (nodePly.getMoveCounter() == 0)
        {
            //cout << "LIMPOU BOARD\n";
            nodePly.limparBoard();
            nodeLevelCounter = 0;
        }

        // nodePly.imprimirBoard();
        nodeAux.copiarBoard(nodePly, nodeLevelCounter+1);

        if (nodeAux.getMoveCounter() % 2 == 1)
            nodeAux.jogarTurnoComputador('O');
        else
            nodeAux.jogarTurnoComputador('X');

        if (nodeAux.getMoveCounter() > 4)
        {
            // TO-DO: TA FALTANDO IDENTIFICAR QUEM GANHOU OU SE EMPATOU
            // COMO PADRAO VAMOS ENSINAR A IA A JOGAR COMO X
            // Ou seja, recompense se X ganhar
            int nodeResult = nodeAux.verificarStatus();
            if (nodeResult > 0)
            {
                if(nodeResult == 1){
                    cout << "BOT LOSES\n";
                    return 0;
                }else
                if(nodeResult == 2){
                    cout << "BOT WON\n";
                    plyChilds.emplace_back(new Ply(nodeAux, nodeLevelCounter+1));
                    return 1;
                }else{
                    return 0;
                }
                // GANHOU OU PERDER - Retornar Recompensa
            }
        }

        if (plyChilds.empty())
        {
            //cout << "CRIOU CHILD\n"; 
            plyChilds.emplace_back(new Ply(nodeAux, nodeLevelCounter+1));
            // REVISAR !!!!! 
            weight += plyChilds.back()->novoJogo();
            return weight;
        }
        else{
            for (int i = 0; i < plyChilds.size(); i++)
            {
                plyChilds.emplace_back(new Ply(nodeAux, nodeLevelCounter+1));
                for (int i = 0; i < plyChilds.size() - 1; i++)
                {
                    if (plyChilds.back()->nodePly.comparaBoards(plyChilds.at(i)->nodePly))
                    {
                         plyChilds.erase(plyChilds.end());
                         return 0;
                    }
                }
                weight += plyChilds.back()->novoJogo();
                return weight;
            }
        }

        return 0;
    }

    // Essa *JOGADA* é aleatoria mas não totalmente
    // Cada node vai ter um weight
    // Sendo os nodes do ultimo level com weight = resultado ( -1 perdeu, 1 ganhou)
    // E nodes acimas a soma dos weights de nivel mais baixo
    // if(nChilds): w = nChilds.sumWeights
    // else: w = getResultOfGame

    // totalMoveCounter = Nivel do Node
};

int main()
{
    srand((unsigned)time(0));

    int playerMoveCounter;

    // OPCOES - Jogar contra Jogador | Assistir Bot contra Bot | Treinar Bot

    Ply lance1;

    cout << lance1.novoJogo();
    cout << lance1.novoJogo();
    cout << lance1.novoJogo();
    cout << lance1.novoJogo();
    cout << lance1.novoJogo();
    cout << lance1.novoJogo();
    cout << lance1.novoJogo();
    cout << lance1.novoJogo();
    cout << lance1.novoJogo();
    cout << lance1.novoJogo();
    cout << lance1.novoJogo();
    cout << lance1.novoJogo();
    cout << lance1.novoJogo();

    return 0;
}