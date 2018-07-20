import bottle
from bottle import route, template, request, error, debug, post, get
from beaker.middleware import SessionMiddleware

from iw4m import m2http, m2

def is_authed():
    s = request.environ.get('beaker.session')
    password = s.get('password', '')
    
    if m2.get_dvar('rcon_password') == '':
        return False
    
    if password == m2.get_dvar("rcon_password"):
        return True
    
    return False

@route('/')
def home():
    return template('home', 
        hostname = m2.get_dvar("sv_hostname"), 
        maxplayers = m2.get_dvar("party_maxplayers"),
        admin_auth = is_authed(),
        clients = m2.get_clients())
        
@route('/kick/<num:int>')
def kick(num):
    if is_authed():
        m2.execute('clientkick %d "Kicked from web interface by TODO"' % num)
    
    bottle.redirect('/')
        
@get('/login')
def login():
    if is_authed():
        bottle.redirect('/')
        
    # todo: possibly allow people with the server key to set an rcon password?
    if m2.get_dvar('rcon_password') == '':
        return template('login', error = 'No rcon_password is set.')
        
    return template('login')
    
@post('/login')
def login_do():
    if is_authed():
        bottle.redirect('/')

    if m2.get_dvar('rcon_password') == '':
        return template('login', error = 'No rcon_password is set.')

    if request.forms.password != m2.get_dvar('rcon_password'):
        return template('login', error = 'Wrong password.')

    s = request.environ.get('beaker.session')
    s['password'] = request.forms.password
    s.save()
    
    bottle.redirect('/')
    
@route('/logout')
def logout():
    if not is_authed():
        return ''
    
    s = request.environ.get('beaker.session')
    s['password'] = ''
    s.save()
    
    bottle.redirect('/')
    
@route('/images/<name>')
def image(name):
    return bottle.static_file(name, root='./main/m2/images/')

bottle.debug()
bottle.TEMPLATE_PATH = ['./main/m2/views/']

session_opts = {
    'session.type': 'file',
    'session.cookie_expires': 600,
    'session.data_dir': './players/tmp/',
    'session.auto': True
}
app = SessionMiddleware(bottle.app(), session_opts)
    
m2http.set_server(app)