

from genilib.configuration import Config as Config

def main():
    config = Config("GeniControl")
    config.load()
    size = (config.get('window', 'sizex'), config.get('window', 'sizey'))
    pos = (config.get('window', 'posx'), config.get('window', 'posy'))
    print("Size: {0}".format(size))
    print("Position: {0}".format(pos))
    #config.save()

if __name__ == '__main__':
    main()
