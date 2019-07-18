Rails.application.routes.draw do
  resources :measures do
    collection do
      get 'remove_all'
    end
  end
  # For details on the DSL available within this file, see http://guides.rubyonrails.org/routing.html

  root 'measures#index'
  
end
