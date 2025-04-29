web

Route::post('/product', [ProductController::class, 'store'])->name('product.store');

Route::get('/product/{product}/edit', [ProductController::class, 'edit'])->name('product.edit');

Route::put('/product/{product}/update', [ProductController::class, 'update'])->name('product.update');
Route::delete('/product/{product}/delete', [ProductController::class, 'delete'])->name('product.delete');



Route::get('/login', [UserController::class, 'showLogin'])->name('show.login');
Route::get('/signin', [UserController::class, 'showSignin'])->name('show.signin');
Route::post('/login', [UserController::class, 'login'])->name('login');
Route::post('/signin', [UserController::class, 'signin'])->name('signin');
Route::post('/logout', [UserController::class, 'logout'])->name('user.logout');
Route::put('/reset',[UserController::class,'resetPassword'])->name('user.reset');
Route::get('/reset',[UserController::class,'showResetPassword'])->name('show.reset');

//product Controller
class ProductController extends Controller
{
public function index(){
$products = product::all();
return view('product.index',['products'=>$products]);
}

public function create(){
return view('product.create');
}

public function store(Request $request){
$data = $request->validate(
[
'name' => 'required',
'qty' => 'required|numeric',
'price' => 'required|numeric',
'description'=> 'nullable',
]);

$newProduct = product::create($data);

return redirect(route('product.index'));

}

public function edit(product $product){
return view('product.edit',['product'=>$product]);
}

public function update(product $product, Request $request){
$data = $request->validate(
[
'name' => 'required',
'qty' => 'required|numeric',
'price' => 'required|numeric',
'description'=> 'nullable',
]);

$product ->update(($data));

return redirect(route('product.index'))->with('success','Product updated successfully');
}

public function delete(product $product){
$product->delete();
return redirect(route('product.index'))->with('success','product deleted succesfully');
}
}


//user UserController
class UserController extends Controller
{
public function showLogin(){
return view('user.login');
}

public function showSignin(){
return view('user.signin');
}

public function signin(Request $request){

$password = $request->input('password');
$password_confirmation = $request->input('password_confirmation');
if ($password !== $password_confirmation) {
return redirect()->back()->with('error', 'Password and confirmation do not match.');
}


$validated = $request->validate([
'name' => 'required|string|max:255',
'email'=> 'required|email|max:255|unique:users',
'password'=> 'required|string',
'password_confirmation'=> 'required|string',
]);

$user = User::create($validated);

Auth::login($user);
return redirect()->route('product.index')->with('success','User Created Successfully');
}

public function login(Request $request){
$validated = $request->validate([
'email' => 'required|email',
'password' => 'required|string',
]);

if(Auth::attempt($validated)){
$request->session()->regenerate();

return redirect()->route('product.index')->with('success','Login Successfully');
}

throw ValidationException::withMessages([
'credentials' => 'The provided credentials do not match our records.'
]);
}

public function logout(Request $request){
Auth::logout();

$request->session()->invalidate();
$request->session()->regenerateToken();

return redirect()->route('show.login');
}

public function showResetPassword(){
return view('user.reset');
}

public function reset(User $user,Request $request){
$validated = $request->validate([
'email'=> 'required|email',
'password'=> 'required|string',
'password_confirmation'=> 'required|string',
]);

if(Auth::attempt($validated)){
$request->session()->regenerateToken();
}
return redirect()->route('user.login');
}
}

public function reset(Request $request){
        $email = Auth::user()->email;
        $user = User::where('email', $email)->first();
        if(Auth::attempt(['email'=>$email, 'password'=>$request->input('prePassword')])){
            if($request->input('password') == $request->input('password2')){
                $user->password = Hash::make($request->input('password2'));
                $user->save();
                return redirect()->route('login');
            }
            return redirect()->back()->with('error', 'password not matches');
        }
        return redirect()->back()->with('error', 'token not set');

    }



//edit form

<form action="{{ route('product.update',['product' => $product]) }}" method="post" value="">

    //delete button

    <form method="post" action="{{ route('product.delete',['product'=>$product]) }}">
        <input type="submit" value="delete">
        @csrf
        @method('DELETE')

    </form>