class MeasuresController < ApplicationController
  before_action :set_measure, only: [:show, :edit, :update, :destroy]
  # GET /measures
  # GET /measures.json
  def index

    require 'net/http'
    require 'json'

    @measures = Measure.all.order("created_at DESC")
    weatherData
    
  end

  # GET /measures/1
  # GET /measures/1.json
  def show
  end

  # GET /measures/new
  def new
    @measure = Measure.new
  end

  # GET /measures/1/edit
  def edit
  end

  # POST /measures
  # POST /measures.json
  # Recieve data from Arduino and save in database
  def create
    @measure = Measure.new(measure_params)
    
    respond_to do |format|
      if @measure.save
        format.html { redirect_to @measure, notice: 'Measure was successfully created.' }
        format.json { render :show, status: :created, location: @measure }
        MeasureCleanupJob.set(wait: 36.hours).perform_later @measure
      else
        format.html { render :new }
        format.json { render json: @measure.errors, status: :unprocessable_entity }
      end
    end
    
  end

  # PATCH/PUT /measures/1
  # PATCH/PUT /measures/1.json
  def update
    respond_to do |format|
      if @measure.update(measure_params)
        format.html { redirect_to @measure, notice: 'Measure was successfully updated.' }
        format.json { render :show, status: :ok, location: @measure }
      else
        format.html { render :edit }
        format.json { render json: @measure.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /measures/1
  # DELETE /measures/1.json
  def destroy
    @measure.destroy
    respond_to do |format|
      format.html { redirect_to measures_url, notice: 'Measure was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  # Remove all table entries 
  # Use for development purposes.
  #GET /measures/remove_all
  def remove_all
    Measure.delete_all
    flash[:notice] = "You have removed all measurements!"
    redirect_to measures_path
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_measure
      @measure = Measure.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def measure_params
      params.require(:measure).permit(:temp_out, :temp_in, :humidity_out, :humidity_in, :soil_moisture)
    end

    def setOnline(state)
      @online = state
    end

    # Method call and processes the DarkSky API to produc local westher forcast
    # Selects teh correct icon to be displayed
    def weatherData 

      icons = {
        "clear-day" => "pe-7w-sun",
        "clear-night" => "pe-7w-moon",
        "rain" => "pe-7w-rain-alt",
        "snow" => "pe-7w-snow-alt",
        "sleet" => "pe-7w-snow",
        "wind" => "pe-7w-wind",
        "fog" => "pe-7w-fog",
        "cloudy" => "pe-7w-cloud",
        "partly-cloudy-day" => "pe-7w-cloud-sun",
        "partly-cloudy-night" => "pe-7w-cloud-moon", 
        "default" => "pe-7w-thermometer-full",
      }
      
      @weather_data = HTTParty.get('https://api.darksky.net/forecast/f3d5dcb788b60bc988305e4ba4bf0fe9/51.208168,-1.516628?exclude=hourly,minutely,flags&units=auto')

    # Current Weather
      @current_time = Time.at(@weather_data ['currently']['time'].to_i)
      @current_summary = @weather_data ['currently']['summary']
      
      @current_icon = icons.fetch(@weather_data ['currently']['icon'],"pe-7w-thermometer-full")
      @current_temp = @weather_data ['currently']['temperature'].to_i
      @current_temp_feels = @weather_data ['currently']['apparentTemperature'].to_i
      @current_humidity = (@weather_data ['currently']['humidity'] * 100).to_i
      @current_windspeed = @weather_data ['currently']['windSpeed'].to_i
      @current_windgust = @weather_data ['currently']['windGust'].to_i
      @current_windbearing = @weather_data ['currently']['windBearing']
      @current_cloudcover = (@weather_data ['currently']['cloudCover'] * 100).to_i
      @current_uvindex = @weather_data ['currently']['uvIndex']
      if @current_uvindex <= 2
        @uvindex = 'bg-succes'
      elsif @current_uvindex <= 5
        @uvindex = 'bg-yellow'
      elsif @current_uvindex <= 7
        @uvindex = 'bg-warning'
      elsif @current_uvindex <= 10
        @uvindex = 'bg-danger'
      elsif @current_uvindex < 10
        @uvindex = 'bg-violet'
      else 
        @uvindex = 'bg-transparent'
      end
      @current_percp = (@weather_data ['currently']['precipProbability'] * 100).to_i
      
      #weather forcast
      @weekly_summary = @weather_data ['daily']['summary']
      # Next 6 days
      @day_one_time = Time.at( @weather_data ['daily']['data'][1]['time'].to_i)
      @day_one_icon = icons [@weather_data ['daily']['data'][1]['icon']]
      @day_one_hi = @weather_data ['daily']['data'][1]['temperatureHigh'].to_i
      @day_one_low = @weather_data ['daily']['data'][1]['temperatureLow'].to_i

      @day_two_time = Time.at( @weather_data ['daily']['data'][2]['time'].to_i)
      @day_two_icon = icons [@weather_data ['daily']['data'][2]['icon']]
      @day_two_hi = @weather_data ['daily']['data'][2]['temperatureHigh'].to_i
      @day_two_low = @weather_data ['daily']['data'][2]['temperatureLow'].to_i

      @day_three_time = Time.at( @weather_data ['daily']['data'][3]['time'].to_i)
      @day_three_icon = icons [@weather_data ['daily']['data'][3]['icon']]
      @day_three_hi = @weather_data ['daily']['data'][3]['temperatureHigh'].to_i
      @day_three_low = @weather_data ['daily']['data'][3]['temperatureLow'].to_i

      @day_four_time = Time.at( @weather_data ['daily']['data'][4]['time'].to_i)
      @day_four_icon = icons [@weather_data ['daily']['data'][4]['icon']]
      @day_four_hi = @weather_data ['daily']['data'][4]['temperatureHigh'].to_i
      @day_four_low = @weather_data ['daily']['data'][4]['temperatureLow'].to_i

      @day_five_time = Time.at( @weather_data ['daily']['data'][5]['time'].to_i)
      @day_five_icon = icons [@weather_data ['daily']['data'][5]['icon']]
      @day_five_hi = @weather_data ['daily']['data'][5]['temperatureHigh'].to_i
      @day_five_low = @weather_data ['daily']['data'][5]['temperatureLow'].to_i

      @day_six_time = Time.at( @weather_data ['daily']['data'][6]['time'].to_i)
      @day_six_icon = icons [@weather_data ['daily']['data'][6]['icon']]
      @day_six_hi = @weather_data ['daily']['data'][6]['temperatureHigh'].to_i
      @day_six_low = @weather_data ['daily']['data'][6]['temperatureLow'].to_i

    end

end
