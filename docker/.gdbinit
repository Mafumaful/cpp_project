python
import sys
sys.path.insert(0, '/home/docker/gdbExtensions')
from printers import register_eigen_printers
register_eigen_printers (None)
end
