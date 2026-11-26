# simOpenMesh API reference

## Alphabetical list

- [simOpenMesh.decimate](#decimate)

## Commands

### <a name="decimate" id="decimate"></a> <a name="simOpenMesh.decimate" id="simOpenMesh.decimate"></a> simOpenMesh.decimate

Generates a decimated version of a shape

#### Synopsis:

```lua
int decimatedShape = simOpenMesh.decimate(int inputShape, map params = nil)
```

```python
int decimatedShape = simOpenMesh.decimate(int inputShape, dict params = None)
```

#### Parameters:

- **shapeHandle** (int): the handle of the shape
- **params** (map, default: nil): a map with optional parameters. Can be nil. Allowed parameters are:
  - percentage (default: 0.8)

#### Return values:

- **decimatedShape** (int): the decimated shape
