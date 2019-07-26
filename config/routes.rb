Rails.application.routes.draw do
  resources :measures do
    collection do
      get 'remove_all'
    end
  end
  # For details on the DSL available within this file, see http://guides.rubyonrails.org/routing.html

  root 'measures#index'

  namespace :charts do
    get "hour_temp_out"
    get "hour_temp_in"
    get "week_temp_out"
    get "week_temp_in"
    get "hour_humidity_out"
    get "hour_humidity_in"
    get "week_humidity_out"
    get "week_humidity_in"
  end
  
end
