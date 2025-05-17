# TIPE

## Utilisation du programme

### Dépendances
- opencv
- g++

### Compilation
Pour compiler le projet
> `make`
Pour nettoyer tout ce qu'a produit la compilation
> `make clean`

### Exécution
> `./output.out <image> <movements-file> <weight> [compression]`
- `image` est le chemin de l'image à traiter.
- `weight` correspond au poids initial entre chaque arête/pixel.
- `compression` est facultatif (par défaut `1`) permet de diviser la taille de l'image avant de la traité.

### Fichiers de mouvement
Format attendu
```csv
start,target,agents
s_y:s_x,t_y:t_x,n
...
```
Où (`s_y`, `s_x`) sont les coordonnées (ligne, colonne) du points de départ, (`t_y`, `t_x`) celles du points d'arrivée, et `n` le nombre d'agents à envoyer.
