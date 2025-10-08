
class FuryBehaviour:
    def __init__(self, _object):
        print('init')
        self.object = _object


class Behaviour(FuryBehaviour):
    def start(self):
        print('start', self.object.worldPosition())
        print(self.object.worldPosition().y)
        print(self.object.textureScales())
        
        print(self.object.shaderName())
        print(self.object.modelName())
        print(self.object.materialName())
        print(self.object.getRays())
    
    def update(self):
        print('update', self.object.worldPosition())
    
    def stop(self):
        print('stop', self.object.worldPosition())

